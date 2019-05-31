#include "include/chain_plugin.hpp"

namespace gruut {

namespace fs = boost::filesystem;

using namespace std;

class TransactionMsgParser {
public:
  optional<Transaction> operator()(const nlohmann::json &transaction_message) {
    Transaction tx;

    bool result = tx.inputMsgTx(transaction_message); // msg_tx 처리. block에서의 txagg_cbor_b64를 처리하는것과는 다름
    if (!result)
      return {};

    return tx;
  }
};

class TransactionMessageVerifier {
public:
  bool operator()(const Transaction &transaction) {
    if (!verifyID(transaction))
      return false;

    if (!verifyEndorserSig(transaction))
      return false;

    if (!verifyUserSig(transaction))
      return false;

    return true;
  }

  bool operator()(Transaction &transaction, const string &world, const string &chain) {
    transaction.setWorld(world);
    transaction.setChain(chain);

    if (!verifyID(transaction))
      return false;

    if (!verifyEndorserSig(transaction))
      return false;

    if (!verifyUserSig(transaction))
      return false;

    return true;
  }

private:
  bool verifyID(const Transaction &transaction) const {
    BytesBuilder tx_id_builder;
    tx_id_builder.appendBase<58>(transaction.getUserId());
    tx_id_builder.append(transaction.getWorld());
    tx_id_builder.append(transaction.getChain());
    tx_id_builder.appendDec(transaction.getTxTime());

    auto receiver_id = transaction.getReceiverId();
    receiver_id.empty() ? tx_id_builder.append("") : tx_id_builder.appendBase<58>(receiver_id);

    tx_id_builder.appendDec(transaction.getFee());
    tx_id_builder.append(transaction.getContractId());
    tx_id_builder.append(transaction.getTxInputCbor());

    vector<uint8_t> tx_id = Sha256::hash(tx_id_builder.getBytes());
    if (transaction.getTxId() != TypeConverter::encodeBase<58>(tx_id)) {
      return false;
    }

    return true;
  }

  bool verifyEndorserSig(const Transaction &transaction) const {
    AGS ags;

    auto &endorsers = transaction.getEndorsers();
    bool result = all_of(endorsers.begin(), endorsers.end(), [&ags, &transaction](const auto &endorser) {
      return ags.verify(endorser.endorser_pk, transaction.getTxId(), endorser.endorser_sig);
    });

    return result;
  }

  bool verifyUserSig(const Transaction &transaction) const {
    AGS ags;

    auto &endorsers = transaction.getEndorsers();
    string message = transaction.getTxId();
    for_each(endorsers.begin(), endorsers.end(),
             [&message](auto &endorser) { message += endorser.endorser_id + endorser.endorser_pk + endorser.endorser_sig; });

    auto user_sig = transaction.getUserSig();

    return ags.verify(transaction.getTxUserPk(), message, user_sig);
  }
};

class TransactionPool {
public:
  void add(const Transaction &tx) {
    {
      unique_lock<shared_mutex> writerLock(pool_mutex);

      tx_pool.try_emplace(tx.getTxId(), tx);
    }
  }

  vector<Transaction> fetchAll() {
    {
      shared_lock<shared_mutex> readerLock(pool_mutex);

      vector<Transaction> v;
      v.reserve(tx_pool.size());

      std::transform(tx_pool.begin(), tx_pool.end(), std::back_inserter(v), [](auto &kv) { return kv.second; });

      return v;
    }
  }

private:
  std::map<string, Transaction> tx_pool;
  std::shared_mutex pool_mutex;
};

class ChainPluginImpl {
public:
  unique_ptr<Chain> chain;
  unique_ptr<TransactionPool> transaction_pool;

  string dbms;
  string table_name;
  string db_user_id;
  string db_password;

  string block_input_path;

  nlohmann::json genesis_state;

  incoming::channels::transaction::channel_type::Handle incoming_transaction_subscription;
  incoming::channels::block::channel_type::Handle incoming_block_subscription;
  incoming::channels::SCE_result::channel_type::Handle incoming_result_subscription;

  void initialize() {
    chain = make_unique<Chain>(dbms, table_name, db_user_id, db_password);
    transaction_pool = make_unique<TransactionPool>();

    chain->startup(genesis_state);
  }

  void pushTransaction(const nlohmann::json &transaction_json) {
    logger::INFO("pushTransaction() Called");
    TransactionMsgParser parser;
    const auto transaction = parser(transaction_json);
    if (!transaction.has_value())
      return;

    TransactionMessageVerifier verfier;
    auto valid = verfier(transaction.value());

    if (valid) {
      transaction_pool->add(transaction.value());
    }
  }

  void pushBlock(const nlohmann::json &block_json) {
    logger::INFO("chain_plugin - pushBlock() Called");

    // TODO: 입력되는 블록 관련 메세지가 BONE이나 PING일 경우의 처리 추가 필요
    // TODO: block 구조체 형태로 입력되는게 맞을지 재고려

    Block input_block;
    input_block.initialize(block_json);

    if (!earlyStage(input_block)) {
      return;
    }

    ubp_push_result_type push_result = chain->pushBlock(input_block);
    if (push_result.height == 0) {
      logger::ERROR("Block input fail: height 0");
      return;
    }
    if (push_result.duplicated) {
      logger::ERROR("Block input fail: duplicated");
      return;
    }

    UnresolvedBlock resolved_block;
    bool resolve_result = chain->resolveBlock(input_block, resolved_block);
    if (resolve_result) {
      chain->applyBlock(resolved_block.block);
      chain->applyTransaction(resolved_block.block);
    }

    return;
  }

  void processTxResult(const nlohmann::json &result) {
    // TODO: state tree lock 필요. head가 달라지면서 state tree가 반영되면 정확한 값이 아니게 될 수 있다
    base58_type block_id = json::get<string>(result["block"], "id").value();
    block_height_type block_height = static_cast<block_height_type>(stoi(json::get<string>(result["block"], "height").value()));
    UnresolvedBlock updated_UR_block = chain->findBlock(block_id, block_height);

    if (updated_UR_block.block.getBlockId() != chain->getCurrentHeadId()) {
      chain->moveHead(updated_UR_block.block.getPrevBlockId(), updated_UR_block.block.getHeight() - 1);
    }

    nlohmann::json results_json = result["results"];
    for (auto &each_result : results_json) {
      result_query_info_type result_info;
      result_info.block_id = block_id;
      result_info.block_height = block_height;
      result_info.tx_id = json::get<string>(each_result, "txid").value();
      result_info.status = json::get<bool>(each_result, "status").value();
      result_info.info = json::get<string>(each_result, "info").value();
      result_info.author = json::get<string>(each_result["authority"], "author").value();
      result_info.user = json::get<string>(each_result["authority"], "user").value();
      result_info.receiver = json::get<string>(each_result["authority"], "receiver").value();
      result_info.self = json::get<string>(each_result["authority"], "self").value();

      nlohmann::json friends_json = each_result["friend"];
      for (auto &each_friend : friends_json) {
        result_info.friends.emplace_back(each_friend.get<string>());
      }
      result_info.fee_author = stoi(json::get<string>(each_result["fee"], "author").value());
      result_info.fee_user = stoi(json::get<string>(each_result["fee"], "user").value());

      nlohmann::json queries_json = each_result["queries"];
      for (auto &each_query : queries_json) {
        string type = json::get<string>(each_query, "type").value();
        nlohmann::json option_json = each_query["option"];
        if (type == "user.join") {
          chain->queryUserJoin(updated_UR_block, option_json, result_info);
        } else if (type == "user.cert") {
          chain->queryUserCert(updated_UR_block, option_json, result_info);
        } else if (type == "contract.new") {
          chain->queryContractNew(updated_UR_block, option_json, result_info);
        } else if (type == "contract.disable") {
          chain->queryContractDisable(updated_UR_block, option_json, result_info);
        } else if (type == "v.incinerate") {
          chain->queryIncinerate(updated_UR_block, option_json, result_info);
        } else if (type == "v.create") {
          chain->queryCreate(updated_UR_block, option_json, result_info);
        } else if (type == "v.transfer") {
          chain->queryTransfer(updated_UR_block, option_json, result_info);
        } else if (type == "scope.user") {
          chain->queryUserScope(updated_UR_block, option_json, result_info);
        } else if (type == "scope.contract") {
          chain->queryContractScope(updated_UR_block, option_json, result_info);
        } else if (type == "trade.item") {
          chain->queryTradeItem(updated_UR_block, option_json, result_info);
        } else if (type == "trade.v") {
          chain->queryTradeVal(updated_UR_block, option_json, result_info);
        } else if (type == "run.query") {
          chain->queryRunQuery(updated_UR_block, option_json, result_info);
        } else if (type == "run.contract") {
          chain->queryRunContract(updated_UR_block, option_json, result_info);
        } else {
          logger::ERROR("URBP, Something error in query process");
        }
      }
    }
  }

  vector<Transaction> getTransactions() {
    return transaction_pool->fetchAll();
  }

  void start() {
    // TODO: msg 관련 요청 감시 (block, ping, request, etc..)

    { // test code start
      // 테스트 시에는 임의로 block_input_test.json의 블록들을 저장하는것부터 시작.
      chain->setPool("11111111111111111111111111111111", 0, 0,
                     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=", "11111111111111111111111111111111");

      nlohmann::json input_block_json;

      fs::path config_path = block_input_path;
      if (config_path.is_relative())
        config_path = fs::current_path() / config_path;

      std::ifstream i(config_path.make_preferred().string());

      i >> input_block_json;

      std::vector<Block> blocks;
      blocks.resize(10);

      for (int i = 0; i < 8; ++i) {
        blocks[i].initialize(input_block_json[i]);

        ubp_push_result_type push_result = chain->pushBlock(blocks[i]);
        if (push_result.height == 0) {
          logger::ERROR("Block input fail: height 0");
          return;
        }
        if (push_result.duplicated) {
          logger::ERROR("Block input fail: duplicated");
          return;
        }

        UnresolvedBlock resolved_block;
        bool resolve_result = chain->resolveBlock(blocks[i], resolved_block);
        if (resolve_result) {
          chain->applyBlock(resolved_block.block);
          chain->applyTransaction(resolved_block.block);
          chain->applyUserLedger(resolved_block.user_ledger_list);
          chain->applyContractLedger(resolved_block.contract_ledger_list);
          chain->applyUserAttribute();
          chain->applyUserCert();
          chain->applyContract();
        }
      }
    } // test code end
  }

  // block verify 코드. chain의 접근권한이 필요함. 정리 필요.
  bool earlyStage(Block &block) {
    if (!verifyBlock(block))
      return false;

    vector<Transaction> transactions = block.getTransactions();
    for (auto &each_transaction : transactions) {
      TransactionMessageVerifier tx_verifier;
      if (!tx_verifier(each_transaction, block.getWorldId(), block.getChainId()))
        return false;
    }

    return true;
  }

  bool lateStage(Block &block) {
    if (!verifyBlockID(block))
      return false;

    if (!verifySSigs(block))
      return false;

    return true;
  }

  bool verifyBlock(const Block &block) {

    // TODO: SSig의 퀄리티, 수 검증 추가
    // TODO: block time validation 추가

    if (!verifyBlockID(block))
      return false;

    vector<hash_t> tx_merkle_tree = makeStaticMerkleTree(block.getTxaggs());
    if (block.getTxRoot() != TypeConverter::encodeBase<64>(tx_merkle_tree.back())) {
      return false;
    }

    if (!verifyBlockHash(block))
      return false;

    vector<Signature> signers = block.getSigners();
    vector<string> signer_id_sigs;
    signer_id_sigs.clear();
    for (auto &each_signer : signers) {
      string id_sig = each_signer.signer_id + each_signer.signer_sig; // need bytebuilder?
      signer_id_sigs.emplace_back(id_sig);
    }
    vector<hash_t> sg_merkle_tree = makeStaticMerkleTree(signer_id_sigs);
    if (block.getSgRoot() != TypeConverter::encodeBase<64>(sg_merkle_tree.back())) {
      return false;
    }

    // TODO: us_state_root 검증 추가
    // TODO: cs_state_root 검증 추가

    verifyProducerSig(block);

    return true;
  }

  bool verifyBlockID(const Block &block) const {
    BytesBuilder block_id_builder;
    block_id_builder.appendBase<58>(block.getBlockProdId());
    block_id_builder.appendDec(block.getBlockTime());
    block_id_builder.append(block.getWorldId());
    block_id_builder.append(block.getChainId());
    block_id_builder.appendDec(block.getHeight());
    block_id_builder.appendBase<58>(block.getPrevBlockId());

    hash_t block_id = Sha256::hash(block_id_builder.getBytes());
    if (block.getBlockId() != TypeConverter::encodeBase<58>(block_id)) {
      return false;
    }

    return true;
  }

  bool verifyBlockHash(const Block &block) const {
    BytesBuilder block_hash_builder;
    block_hash_builder.appendBase<58>(block.getBlockId());
    block_hash_builder.appendBase<64>(block.getTxRoot());
    block_hash_builder.appendBase<64>(block.getUserStateRoot());
    block_hash_builder.appendBase<64>(block.getContractStateRoot());

    hash_t block_hash = Sha256::hash(block_hash_builder.getBytes());
    if (block.getBlockHash() != TypeConverter::encodeBase<64>(block_hash)) {
      return false;
    }
    return true;
  }

  bool verifyProducerSig(const Block &block) const {
    BytesBuilder prod_sig_builder;
    prod_sig_builder.appendDec(block.getBlockPubTime());
    prod_sig_builder.appendBase<64>(block.getBlockHash());
    prod_sig_builder.appendDec(block.getNumTransaction()); // 32bit? 64bit?
    prod_sig_builder.appendDec(block.getNumSigners());     // 32bit? 64bit?
    prod_sig_builder.appendBase<64>(block.getSgRoot());

    string message = TypeConverter::bytesToString(prod_sig_builder.getBytes());
    string producer_cert = chain->getUserCert(block.getBlockProdId());
    AGS ags;
    return ags.verifyPEM(producer_cert, message, block.getBlockProdSig());
  }

  std::vector<hash_t> makeStaticMerkleTree(const std::vector<string> &material) {

    std::vector<hash_t> merkle_tree_vector;
    std::vector<hash_t> sha256_material;

    // 입력으로 들어오는 material 벡터는 해시만 하면 되도록 처리된 상태로 전달되어야 합니다
    for (auto &each_element : material) {
      sha256_material.emplace_back(Sha256::hash(each_element));
    }

    StaticMerkleTree merkle_tree;
    merkle_tree.generate(sha256_material);
    merkle_tree_vector = merkle_tree.getStaticMerkleTree();

    return merkle_tree_vector;
  }

  bytes getUserStateRoot() {
    return chain->getUserStateRoot();
  }

  bytes getContractStateRoot() {
    return chain->getContractStateRoot();
  }

  bool verifySSigs(const Block &block) const {
    BytesBuilder signer_sig_builder;
    signer_sig_builder.appendBase<58>(block.getBlockId());
    signer_sig_builder.appendBase<64>(block.getTxRoot());
    signer_sig_builder.appendBase<64>(block.getUserStateRoot());
    signer_sig_builder.appendBase<64>(block.getContractStateRoot());

    hash_t block_info_hash = Sha256::hash(signer_sig_builder.getBytes());
    string message = TypeConverter::bytesToString(block_info_hash);
    vector<Signature> signers = block.getSigners();

    AGS ags;
    for (auto &each_signer : signers) {
      string signer_cert = chain->getUserCert(each_signer.signer_id);
      if (!ags.verifyPEM(signer_cert, message, each_signer.signer_sig))
        return false;
    }

    return true;
  }
  // block verify part end
};

ChainPlugin::ChainPlugin() : impl(make_unique<ChainPluginImpl>()) {}

void ChainPlugin::pluginInitialize(const boost::program_options::variables_map &options) {
  logger::INFO("ChainPlugin Initialize");

  if (options.count("world-create") > 0) {
    fs::path config_path = options.at("world-create").as<std::string>();

    if (config_path.is_relative())
      config_path = fs::current_path() / config_path;

    if (!fs::exists(config_path)) {
      logger::ERROR("Can't find a world create file. (path: {})", config_path.string());
      throw std::invalid_argument("Cannot find a world_create.json"s);
    }

    std::ifstream i(config_path.make_preferred().string());

    i >> impl->genesis_state;
  } else {
    throw std::invalid_argument("world create file does not exist"s);
  }

  if (options.count("dbms")) {
    impl->dbms = options.at("dbms").as<string>();
  } else {
    throw std::invalid_argument("the input of dbms is empty"s);
  }

  if (options.count("table-name")) {
    impl->table_name = options.at("table-name").as<string>();
  } else {
    throw std::invalid_argument("the input of table_name is empty"s);
  }

  if (options.count("database-user")) {
    impl->db_user_id = options.at("database-user").as<string>();
  } else {
    throw std::invalid_argument("the input of database's user_id is empty"s);
  }

  if (options.count("database-password")) {
    impl->db_password = options.at("database-password").as<string>();
  } else {
    throw std::invalid_argument("the input of database's password is empty"s);
  }

  if (options.count("block-input-path")) {
    impl->block_input_path = options.at("block-input-path").as<string>();
  } else {
    throw std::invalid_argument("the input of block input path is empty"s);
  }

  auto &transaction_channel = app().getChannel<incoming::channels::transaction::channel_type>();
  impl->incoming_transaction_subscription =
      transaction_channel.subscribe([this](const nlohmann::json &transaction) { impl->pushTransaction(transaction); });

  auto &block_channel = app().getChannel<incoming::channels::block::channel_type>();
  impl->incoming_block_subscription = block_channel.subscribe([this](const nlohmann::json &block) { impl->pushBlock(block); });

  auto &SCE_result_channel = app().getChannel<incoming::channels::SCE_result::channel_type>();
  impl->incoming_result_subscription =
      SCE_result_channel.subscribe([this](const nlohmann::json &result) { impl->processTxResult(result); });

  impl->initialize();
}

// clang-format off
void ChainPlugin::setProgramOptions(options_description &cfg) {
  cfg.add_options()("world-create", boost::program_options::value<string>()->composing(), "the location of a world_create.json file")
  ("dbms", boost::program_options::value<string>()->composing(), "DBMS (MYSQL)")("table-name", boost::program_options::value<string>()->composing(), "table name")
  ("database-user", boost::program_options::value<string>()->composing(), "database user id")
  ("database-password", boost::program_options::value<string>()->composing(), "database password")
  ("block-input-path", boost::program_options::value<string>()->composing(), "block input json path");
}
// clang-format on

void ChainPlugin::asyncFetchTransactionsFromPool() {
  auto transactions = impl->getTransactions();
  app().getChannel<incoming::channels::transaction_pool::channel_type>().publish(transactions);
}

Chain &ChainPlugin::chain() {
  return *(impl->chain);
}

void ChainPlugin::pluginStart() {
  logger::INFO("ChainPlugin Start");

  impl->start();
}

ChainPlugin::~ChainPlugin() {
  impl.reset();
}
} // namespace gruut
