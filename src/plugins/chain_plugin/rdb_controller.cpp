#include "include/rdb_controller.hpp"
#include "../../../lib/tethys-utils/src/ags.hpp"
#include "mysql/soci-mysql.h"
#include <regex>

using namespace std;

namespace tethys {

RdbController::RdbController(string_view dbms, string_view table_name, string_view db_user_id, string_view db_password)
    : m_dbms(dbms), m_table_name(table_name), m_db_user_id(db_user_id), m_db_password(db_password),
      m_db_pool(config::DB_SESSION_POOL_SIZE) {
  for (int i = 0; i != config::DB_SESSION_POOL_SIZE; ++i) {
    soci::session &sql = m_db_pool.at(i);
    sql.open(m_dbms, "service=" + m_table_name + " user=" + m_db_user_id + " password=" + m_db_password);
  }
  logger::INFO("DB pool initialize");
}

soci::connection_pool &RdbController::pool() {
  return m_db_pool;
}

const vector<contract_id_type> RdbController::queryContractScan(const nlohmann::json &where_json) {
  vector<contract_id_type> contract_id_list;
  try {
    // TODO: 네 가지 모두가 아닌 일부만 주어졌을 경우의 상황 처리 필요
    string desc = json::get<string>(where_json, "desc").value();
    base58_type author = json::get<string>(where_json, "author").value();
    timestamp_t after = static_cast<tethys::timestamp_t>(stoll(json::get<string>(where_json, "after").value()));
    timestamp_t before = static_cast<tethys::timestamp_t>(stoll(json::get<string>(where_json, "before").value()));

    contract_id_type cid;

    // clang-format off
      soci::row result;
      soci::session db_session(RdbController::pool());
      soci::statement st = (db_session.prepare << "SELECT cid from contracts WHERE desc = :desc, author = :author, after = :after, before = :before", soci::use(desc, "desc"), soci::use(author, "author"),soci::use(after, "after"),soci::use(before, "before"), soci::into(result));
      st.execute(true);
    // clang-format on
    do {
      result >> cid;

      contract_id_list.emplace_back(cid);
    } while (st.fetch());

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return contract_id_list;
  } catch (...) {
    logger::ERROR("Unexpected error at `queryContractScan`");
    return contract_id_list;
  }
}

const string RdbController::queryContractGet(const nlohmann::json &where_json) {
  string contract;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const vector<user_cert_type> RdbController::queryCertGet(const nlohmann::json &where_json) {
  vector<user_cert_type> cert_list;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const user_attribute_type RdbController::queryUserInfoGet(const nlohmann::json &where_json) {
  user_attribute_type user_info;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const vector<user_ledger_type> RdbController::queryUserScopeGet(const nlohmann::json &where_json) {
  vector<user_ledger_type> user_ledger_list;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const vector<contract_ledger_type> RdbController::queryContractScopeGet(const nlohmann::json &where_json) {
  vector<contract_ledger_type> contract_ledger_list;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const Block RdbController::queryBlockGet(const nlohmann::json &where_json) {
  Block block;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const Transaction RdbController::queryTxGet(const nlohmann::json &where_json) {
  Transaction transaction;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const vector<base58_type> RdbController::queryBlockScan(const nlohmann::json &where_json) {
  vector<base58_type> block_id_list;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

const vector<base58_type> RdbController::queryTxScan(const nlohmann::json &where_json) {
  vector<base58_type> tx_id_list;
  try {

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return;
  } catch (...) {
    logger::ERROR("Unexpected error at ``");
    return;
  }
}

bool RdbController::applyBlockToRDB(const Block &block) {
  logger::INFO("insert Block Data");

  soci::row result;
  soci::session db_session(RdbController::pool());

  try {
    // clang-format off
  string block_id = block.getBlockId();
  string block_hash = block.getBlockHash();
  string prev_block_id = block.getPrevBlockId();
  string pre_block_sig = block.getPrevBlockSig();
  string block_prod_id = block.getBlockProdId();
  string block_pro_sig = block.getBlockProdSig();
  string tx_id = block.getTransactions()[0].getTxId();
  string tx_root = block.getTxRoot();
  string user_state_root = block.getUserStateRoot();
  string contract_state_root = block.getContractStateRoot();
  string sig_root = block.getSgRoot();
  string block_cert = block.getBlockCert();

  soci::statement st = (db_session.prepare << "INSERT INTO blocks (block_id, block_height, block_hash, block_time, block_pub_time, block_prev_id, block_link, producer_id, producer_sig, txs, tx_root, us_state_root, cs_state_root, sg_root, certificate) VALUES (:block_id, :block_height, :block_hash, :block_time, :block_pub_time, :block_prev_id, :block_link, :producer_id, :producer_sig, :txs, :tx_root, :us_state_root, :cs_state_root, :sg_root, :certificate)",
      soci::use(block_id), soci::use(block.getHeight()), soci::use(block_hash),
      soci::use(block.getBlockTime()), soci::use(block.getBlockPubTime()), soci::use(prev_block_id),
      soci::use(pre_block_sig), soci::use(block_prod_id),
      soci::use(block_pro_sig), soci::use(tx_id),
      soci::use(tx_root), soci::use(user_state_root),
      soci::use(contract_state_root), soci::use(sig_root),
      soci::use(block_cert), soci::into(result));
    // clang-format on
    st.execute(true);
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return false;
  } catch (...) {
    logger::ERROR("Unexpected error at `applyBlockToRDB`");
    return false;
  }
  return true;
}

bool RdbController::applyTransactionToRDB(const Block &block) {
  logger::INFO("insert Transaction Data");

  soci::row result;
  soci::session db_session(RdbController::pool());
  for (auto &each_transaction : block.getTransactions())
    try {
      // clang-format off
        string tx_id = each_transaction.getTxId();
        string tx_contract_id = each_transaction.getContractId();
        string tx_user = each_transaction.getUserId();
        string tx_user_pk = each_transaction.getTxUserPk();
        string tx_receiver = each_transaction.getReceiverId();
        string tx_input = TypeConverter::bytesToString(each_transaction.getTxInputCbor());
        string tx_agg_cbor = each_transaction.getTxAggCbor();
        string tx_block_id = block.getBlockId();

        soci::statement st = (db_session.prepare << "INSERT INTO transactions (tx_id, tx_time, tx_contract_id, tx_fee_author, tx_fee_user, tx_user, tx_user_pk, tx_receiver, tx_input, tx_agg_cbor, block_id, tx_pos) VALUES (:tx_id, :tx_time, :tx_contract_id, :tx_fee_author, :tx_fee_user, :tx_user, :tx_user_pk, :tx_receiver, :tx_input, :tx_agg_cbor, :block_id, :tx_pos)",
            soci::use(tx_id), soci::use(each_transaction.getTxTime()), soci::use(tx_contract_id),
            soci::use(each_transaction.getFee()), soci::use(each_transaction.getFee()), soci::use(tx_user),
            soci::use(tx_user_pk), soci::use(tx_receiver),
            soci::use(tx_input), soci::use(tx_agg_cbor),
            soci::use(tx_block_id), soci::use(each_transaction.getTxPos()),
            soci::into(result));
      // clang-format on
      st.execute(true);
    } catch (soci::mysql_soci_error const &e) {
      logger::ERROR("MySQL error: {}", e.what());
      return false;
    } catch (...) {
      logger::ERROR("Unexpected error at `applyTransactionToRDB`");
      return false;
    }
  return true;
}

bool RdbController::applyUserLedgerToRDB(const map<string, user_ledger_type> &user_ledger_list) {
  try {
    string var_name;
    string var_val;
    int var_type;
    base58_type uid;
    timestamp_t up_time;
    block_height_type up_block;
    string tag;
    string pid;
    QueryType query_type;

    soci::row result;
    soci::session db_session(RdbController::pool());

    // TODO: '통화'속성의 변수는 변경 불가능한 조건 검사 시행

    for (auto &each_ledger : user_ledger_list) {
      soci::statement st(db_session);

      var_name = each_ledger.second.var_name;
      var_val = each_ledger.second.var_val;
      var_type = each_ledger.second.var_type;
      uid = each_ledger.second.uid;
      up_time = each_ledger.second.up_time;
      up_block = each_ledger.second.up_block;
      tag = each_ledger.second.tag;
      pid = each_ledger.second.pid;
      query_type = each_ledger.second.query_type;

      // clang-format off
      if (query_type == QueryType::INSERT) {
        // TODO: 변경 가능한 column이 어떤 것이고, 제약을 어떻게 걸어야 하는지 검토하여 반영 필요
        st = (db_session.prepare << "INSERT INTO user_scope (var_name, var_value, var_type, var_owner, up_time, up_block, tag, pid) VALUES (:var_name, :var_value, :var_type, :var_owner, :up_time, :up_block, :tag, :pid)",
                soci::use(var_name, "var_name"), soci::use(var_val, "var_value"), soci::use(var_type, "var_type"),
                soci::use(uid, "var_owner"), soci::use(up_time, "up_time"), soci::use(up_block, "up_block"),
                soci::use(tag, "tag"), soci::use(pid, "pid"),
                soci::into(result));
      } else if (query_type == QueryType::UPDATE) {
        st = (db_session.prepare << "UPDATE user_scope SET var_value = :var_value, up_time = :up_time, up_block = :up_block WHERE pid = :pid",
            soci::use(var_val, "var_value"), soci::use(up_time, "up_time"), soci::use(up_block, "up_block"), soci::use(pid, "pid"),
            soci::into(result));
      } else if ((query_type == QueryType::DELETE) && (var_val.empty())) {
        st = (db_session.prepare << "DELETE FROM user_scope WHERE pid = :pid",
                soci::use(pid, "pid"), soci::into(result));
      } else
        logger::ERROR("Error at applyUserLedgerToRDB");
      // clang-format on

      st.execute(true);
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return false;
  } catch (...) {
    logger::ERROR("Unexpected error at applyUserLedgerToRDB");
    return false;
  }
  return true;
}

bool RdbController::applyContractLedgerToRDB(const map<string, contract_ledger_type> &contract_ledger_list) {
  try {
    string var_name;
    string var_val;
    int var_type;
    contract_id_type cid;
    timestamp_t up_time;
    block_height_type up_block;
    string var_info;
    string pid;
    QueryType query_type;

    soci::row result;
    soci::session db_session(RdbController::pool());

    // TODO: '통화'속성의 변수는 변경 불가능한 조건 검사 시행

    for (auto &each_ledger : contract_ledger_list) {
      soci::statement st(db_session);

      cid = each_ledger.second.cid;
      var_name = each_ledger.second.var_name;
      var_val = each_ledger.second.var_val;
      var_type = each_ledger.second.var_type;
      up_time = each_ledger.second.up_time;
      up_block = each_ledger.second.up_block;
      var_info = each_ledger.second.var_info;
      pid = each_ledger.second.pid;
      query_type = each_ledger.second.query_type;

      // clang-format off
      if (query_type == QueryType::INSERT) {
        st = (db_session.prepare << "INSERT INTO contract_scope (contract_id, var_name, var_value, var_type, var_info, up_time, up_block, pid) VALUES (:contract_id, :var_name, :var_value, :var_type, :var_info, :up_time, :up_block, :pid)",
            soci::use(cid, "contract_id"), soci::use(var_name, "var_name"), soci::use(var_val, "var_value"),
            soci::use(var_type, "var_type"), soci::use(var_info, "var_info"), soci::use(up_time, "up_time"),
            soci::use(up_block, "up_block"), soci::use(pid, "pid"),
            soci::into(result));
      } else if (query_type == QueryType::UPDATE) {
        // TODO: 변경 가능한 column이 어떤 것이고, 제약을 어떻게 걸어야 하는지 검토하여 반영 필요
        st = (db_session.prepare << "UPDATE contract_scope SET var_value = :var_value, up_time = :up_time, up_block = :up_block WHERE pid = :pid",
            soci::use(var_val, "var_value"), soci::use(up_time, "up_time"), soci::use(up_block, "up_block"), soci::use(pid, "pid"),
            soci::into(result));
      } else if ((query_type == QueryType::DELETE) && (var_val.empty())) {
        st = (db_session.prepare << "DELETE FROM contract_scope WHERE pid = :pid",
            soci::use(pid, "pid"), soci::into(result));
      } else
        logger::ERROR("Error at applyContractLedgerToRDB");
      // clang-format on

      st.execute(true);
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return false;
  } catch (...) {
    logger::ERROR("Unexpected error at applyContractLedgerToRDB");
    return false;
  }
  return true;
}

bool RdbController::applyUserAttributeToRDB(const map<base58_type, user_attribute_type> &user_attribute_list) {
  try {
    base58_type uid;
    timestamp_t register_day;
    string register_code;
    int gender;
    string isc_type;
    string isc_code;
    string location;
    int age_limit;
    string sigma;

    soci::row result;
    soci::session db_session(RdbController::pool());

    for (auto &each_attribute : user_attribute_list) {
      soci::statement st(db_session);

      uid = each_attribute.second.uid;
      register_day = each_attribute.second.register_day;
      register_code = each_attribute.second.register_code;
      gender = each_attribute.second.gender;
      isc_type = each_attribute.second.isc_type;
      isc_code = each_attribute.second.isc_code;
      location = each_attribute.second.location;
      age_limit = each_attribute.second.age_limit;
      sigma = each_attribute.second.sigma;

      // clang-format off
      // TODO: 변경 가능한 column이 어떤 것이고, 제약을 어떻게 걸어야 하는지 검토하여 반영 필요
      //  QueryType를 적용시켜야 하는지 검토
      st = (db_session.prepare << "INSERT INTO user_attributes (uid, register_day, register_code, gender, isc_type, isc_code, location, age_limit, sigma) VALUES (:uid, :register_day, :register_code, :gender, :isc_type, :isc_code, :location, :age_limit, :sigma) ON DUPLICATE KEY UPDATE register_day = :register_day, register_code = :register_code, gender = :gender, isc_type = :isc_type, isc_code = :isc_code, location = :location, age_limit = :age_limit, sigma = :sigma",
          soci::use(uid, "uid"), soci::use(register_day, "register_day"), soci::use(register_code, "register_code"),
          soci::use(gender, "gender"), soci::use(isc_type, "isc_type"), soci::use(isc_code, "isc_code"),
          soci::use(location, "location"), soci::use(age_limit, "age_limit"), soci::use(sigma, "sigma"),
          soci::into(result));
      // clang-format on

      st.execute(true);
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return false;
  } catch (...) {
    logger::ERROR("Unexpected error at applyUserAttributeToRDB");
    return false;
  }
  return true;
}

bool RdbController::applyUserCertToRDB(const map<base58_type, user_cert_type> &user_cert_list) {
  try {
    base58_type uid;
    string sn;
    timestamp_t nvbefore;
    timestamp_t nvafter;
    string x509;

    soci::row result;
    soci::session db_session(RdbController::pool());

    for (auto &each_cert : user_cert_list) {
      soci::statement st(db_session);

      uid = each_cert.second.uid;
      sn = each_cert.second.sn;
      nvbefore = each_cert.second.nvbefore;
      nvafter = each_cert.second.nvafter;
      x509 = each_cert.second.x509;

      // clang-format off
      st = (db_session.prepare << "INSERT INTO user_certificates (uid, sn, nvbefore, nvafter, x509) VALUES (:uid, :sn, :nvbefore, :nvafter, :x509)",
            soci::use(uid, "uid"), soci::use(sn, "sn"),
            soci::use(nvbefore, "nvbefore"), soci::use(nvafter, "nvafter"), soci::use(x509, "x509"),
            soci::into(result));
      // clang-format on

      st.execute(true);
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return false;
  } catch (...) {
    logger::ERROR("Unexpected error at applyUserCertToRDB");
    return false;
  }
  return true;
}

bool RdbController::applyContractToRDB(const map<contract_id_type, contract_type> &contract_list) {
  try {
    contract_id_type cid;
    timestamp_t after;
    timestamp_t before;
    base58_type author;
    contract_id_type friends;
    string contract;
    string desc;
    string sigma;
    QueryType query_type;

    soci::row result;
    soci::session db_session(RdbController::pool());

    for (auto &each_contract : contract_list) {
      soci::statement st(db_session);

      cid = each_contract.second.cid;
      after = each_contract.second.after;
      before = each_contract.second.before;
      author = each_contract.second.author;
      // TODO: friend 추가할 때 자신을 friend로 추가한 contract를 찾아서 추가해야함을 주의
      friends = each_contract.second.friends;
      contract = each_contract.second.contract;
      desc = each_contract.second.desc;
      sigma = each_contract.second.sigma;
      query_type = each_contract.second.query_type;

      // clang-format off
      if (query_type == QueryType::INSERT) {
        st = (db_session.prepare << "INSERT INTO contracts (cid, after, before, author, friends, contract, desc, sigma) VALUES (:cid, :after, :before, :author, :friends, :contract, :desc, :sigma)",
            soci::use(cid, "cid"), soci::use(after, "after"), soci::use(before, "before"),
            soci::use(author, "author"), soci::use(friends, "friends"), soci::use(contract, "contract"),
            soci::use(desc, "desc"), soci::use(sigma, "sigma"),
            soci::into(result));
      } else if (query_type == QueryType::UPDATE) {
        st = (db_session.prepare << "UPDATE contracts SET before = :before WHERE cid = :cid",
            soci::use(before, "before"), soci::use(cid, "cid"),
            soci::into(result));
      } else
        logger::ERROR("Error at applyContractToRDB");
      // clang-format on

      st.execute(true);
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return false;
  } catch (...) {
    logger::ERROR("Unexpected error at applyContractToRDB");
    return false;
  }
  return true;
}

vector<Block> RdbController::getBlocks(const string &condition) {
  try {
    soci::session db_session(RdbController::pool());

    soci::rowset<> rs(db_session.prepare << "select * from blocks where " + condition);
    vector<Block> blocks;

    for (auto it = rs.begin(); it != rs.end(); ++it) {
      soci::row const &row = *it;
      Block block = rowToBlock(row);
      blocks.push_back(block);
    }

    return blocks;
  } catch (const std::exception &e) {
    logger::ERROR("Failed to get blocks: {}", e.what());

    throw e;
  }
}

optional<Block> RdbController::getBlock(const string &condition) {
  try {
    soci::session db_session(RdbController::pool());
    soci::row r;

    db_session << "select * from blocks " + condition, into(r);
    auto block = rowToBlock(r);

    return block;
  } catch (const std::exception &e) {
    logger::ERROR("Failed to get blocks: {}", e.what());

    return {};
  }
}

Block RdbController::rowToBlock(const soci::row &r) {
  Block block;

  block.setBlockId(r.get<base58_type>("block_id"));
  block.setHeight(r.get<block_height_type>("block_height"));
  block.setBlockHash(r.get<base64_type>("block_hash"));
  block.setBlockTime(r.get<long long>("block_time"));
  block.setBlockPubTime(r.get<long long>("block_pub_time"));
  block.setBlockPrevId(r.get<base58_type>("block_prev_id"));
  block.setBlockPrevSig(r.get<base64_type>("block_link"));
  block.setProducerId(r.get<base58_type>("producer_id"));
  block.setProducerSig(r.get<base64_type>("producer_sig"));
  // TODO: TxID
  block.setTxRoot(r.get<string>("tx_root"));
  block.setUsStateRoot(r.get<string>("us_state_root"));
  block.setCsStateRoot(r.get<string>("cs_state_root"));
  block.setSgRoot(r.get<string>("sg_root"));

  nlohmann::json certificates = nlohmann::json(r.get<string>("certificate"));
  block.setUserCerts(certificates);

  return block;
}

string RdbController::getUserCert(const base58_type &user_id) {
  try {
    string user_cert = {};
    soci::session db_session(RdbController::pool());
    soci::statement st = (db_session.prepare << "select x509 from user_certificates where uid = " + user_id, soci::into(user_cert));
    st.execute(true);

    return user_cert;
  } catch (const std::exception &e) {
    logger::ERROR("Failed to get user_cert: {}", e.what());
    return string();
  }
}

// bool RdbController::queryRunQuery(std::vector<LedgerRecord> &mem_ledger, nlohmann::json &option, result_query_info_type &result_info) {
//  string type = json::get<string>(option, "type").value();
//  nlohmann::json query = option["query"];
//  timestamp_t after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));
//
//  if ((type == "run.query") || (type == "user.cert")) {
//    logger::ERROR("run.query cannot excute 'run.query' or 'user.cert'!");
//    return false;
//  }
//  // TODO: Scheduler에게 지연 처리 요청 전송
//  return true;
//}
//
// bool RdbController::queryRunContract(std::vector<LedgerRecord> &mem_ledger, nlohmann::json &option, result_query_info_type &result_info)
// {
//  contract_id_type cid = json::get<string>(option, "cid").value();
//  string input = json::get<string>(option, "input").value();
//  timestamp_t after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));
//
//  // TODO: authority.user를 현재 user로 대체하여 Scheduler에게 요청 전송
//  return true;
//}

int RdbController::getVarTypeFromRDB(const string &var_owner, const string &var_name) {
  try {
    short var_type = 0;
    soci::row result;
    soci::session db_session(RdbController::pool());
    soci::statement st(db_session);

    // clang-format off
    if(isUserId(var_owner))
      st = (db_session.prepare << "SELECT var_type from user_scope WHERE var_owner = :var_owner AND var_name = :var_name AND tag is NULL", soci::use(var_owner, "var_owner"), soci::use(var_name, "var_name"), soci::into(result));
    else if(isContractId(var_owner))
      st = (db_session.prepare << "SELECT var_type from contract_scope WHERE contract_id = :contract_id AND var_name = :var_name AND var_info is NULL", soci::use(var_owner, "contract_id"), soci::use(var_name, "var_name"), soci::into(result));
    st.execute(true);
    // clang-format on

    result >> var_type;

    int num_rows = (int)st.get_affected_rows();
    if (num_rows == 1)
      return var_type;
    else if (num_rows == 0)
      return (int)UniqueCheck::NO_VALUE;
    else
      return (int)UniqueCheck::NOT_UNIQUE;

  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return -3;
  } catch (...) {
    logger::ERROR("Unexpected error at `getVarTypeFromRDB`");
    return -3;
  }
}

user_ledger_type RdbController::findUserScopeFromRDB(const string &pid) {
  user_ledger_type user_ledger;
  try {
    string var_name, var_value, var_owner, tag, pid;
    short var_type;
    long long up_time;
    int up_block;

    // clang-format off
    soci::row result;
    soci::session db_session(RdbController::pool());
    soci::statement st = (db_session.prepare << "SELECT var_name, var_value, var_type, var_owner, up_time, up_block, tag, pid from user_scope WHERE pid = :pid", soci::use(pid, "pid"), soci::into(result));
    st.execute(true);
    // clang-format on

    result >> var_name >> var_value >> var_type >> var_owner >> up_time >> up_block >> tag >> pid;

    int num_rows = (int)st.get_affected_rows();
    if (num_rows == 1) {
      user_ledger.var_name = var_name;
      user_ledger.var_val = var_value;
      user_ledger.var_type = (int)var_type;
      user_ledger.uid = var_owner;
      user_ledger.up_time = (uint64_t)up_time;
      user_ledger.up_block = (block_height_type)up_block;
      user_ledger.tag = tag;
      user_ledger.pid = pid;
      user_ledger.is_empty = false;
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
  } catch (...) {
    logger::ERROR("Unexpected error at `findUserScopeFromRDB`");
  }
  return user_ledger;
}

contract_ledger_type RdbController::findContractScopeFromRDB(const string &pid) {
  contract_ledger_type contract_ledger;
  try {
    string var_name, var_value, contract_id, var_info, pid;
    short var_type;
    long long up_time;
    int up_block;

    // clang-format off
    soci::row result;
    soci::session db_session(RdbController::pool());
    soci::statement st = (db_session.prepare << "SELECT var_name, var_value, var_type, contract_id, up_time, up_block, var_info, pid from contract_scope WHERE pid = :pid", soci::use(pid, "pid"), soci::into(result));
    st.execute(true);
    // clang-format on

    result >> var_name >> var_value >> var_type >> contract_id >> up_time >> up_block >> var_info >> pid;

    int num_rows = (int)st.get_affected_rows();
    if (num_rows == 1) {
      contract_ledger.var_name = var_name;
      contract_ledger.var_val = var_value;
      contract_ledger.var_type = (int)var_type;
      contract_ledger.cid = contract_id;
      contract_ledger.up_time = (uint64_t)up_time;
      contract_ledger.up_block = (block_height_type)up_block;
      contract_ledger.var_info = var_info;
      contract_ledger.pid = pid;
      contract_ledger.is_empty = false;
    }
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
  } catch (...) {
    logger::ERROR("Unexpected error at `findContractScopeFromRDB`");
  }
  return contract_ledger;
}

vector<user_ledger_type> RdbController::getAllUserLedger() {
  vector<user_ledger_type> user_ledger_list;
  try {
    string var_name, var_value, var_owner, tag, pid;
    short var_type;
    long long up_time;
    int up_block;

    // clang-format off
    soci::row result;
    soci::session db_session(RdbController::pool());
    soci::statement st = (db_session.prepare << "SELECT var_name, var_value, var_type, var_owner, up_time, up_block, tag, pid from user_scope", soci::into(result));
    st.execute(true);
    // clang-format on
    do {
      result >> var_name >> var_value >> var_type >> var_owner >> up_time >> up_block >> tag >> pid;

      user_ledger_type user_ledger;
      user_ledger.var_name = var_name;
      user_ledger.var_val = var_value;
      user_ledger.var_type = (int)var_type;
      user_ledger.uid = var_owner;
      user_ledger.up_time = (uint64_t)up_time;
      user_ledger.up_block = (block_height_type)up_block;
      user_ledger.tag = tag;
      user_ledger.pid = pid;
      user_ledger_list.emplace_back(user_ledger);
    } while (st.fetch());
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return user_ledger_list;
  } catch (...) {
    logger::ERROR("Unexpected error at `getAllUserLedger`");
    return user_ledger_list;
  }
  return user_ledger_list;
}

vector<contract_ledger_type> RdbController::getAllContractLedger() {
  vector<contract_ledger_type> contract_ledger_list;
  try {
    string var_name, var_value, contract_id, var_info, pid;
    short var_type;
    long long up_time;
    int up_block;

    // clang-format off
    soci::row result;
    soci::session db_session(RdbController::pool());
    soci::statement st = (db_session.prepare << "SELECT var_name, var_value, var_type, contract_id, up_time, up_block, var_info, pid from contract_scope", soci::into(result));
    st.execute(true);
    // clang-format on
    do {
      result >> var_name >> var_value >> var_type >> contract_id >> up_time >> up_block >> var_info >> pid;

      contract_ledger_type contract_ledger;
      contract_ledger.var_name = var_name;
      contract_ledger.var_val = var_value;
      contract_ledger.var_type = (int)var_type;
      contract_ledger.cid = contract_id;
      contract_ledger.up_time = (uint64_t)up_time;
      contract_ledger.up_block = (block_height_type)up_block;
      contract_ledger.var_info = var_info;
      contract_ledger.pid = pid;
      contract_ledger_list.emplace_back(contract_ledger);
    } while (st.fetch());
  } catch (soci::mysql_soci_error const &e) {
    logger::ERROR("MySQL error: {}", e.what());
    return contract_ledger_list;
  } catch (...) {
    logger::ERROR("Unexpected error at `getAllContractLedger`");
    return contract_ledger_list;
  }
  return contract_ledger_list;
}

bool RdbController::isUserId(const string &id) {
  const auto BASE58_REGEX = "^[A-HJ-NP-Za-km-z1-9]*$";
  regex rgx(BASE58_REGEX);
  if (id.size() != static_cast<int>(44) || !regex_match(id, rgx)) {
    logger::ERROR("Invalid user ID.");
    return false;
  }
  return true;
}

bool RdbController::isContractId(const string &id) {
  // TODO: 검증 추가
  if (id.size() > 45)
    return true;
  else
    return false;
}

} // namespace tethys