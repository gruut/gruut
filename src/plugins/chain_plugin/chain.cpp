#include "include/chain.hpp"
#include "../../../lib/appbase/include/application.hpp"
#include "../../../lib/tethys-utils/src/ags.hpp"
#include "../../../lib/tinyxml/include/tinyxml2.h"
#include <boost/filesystem.hpp>
#include <regex>

namespace tethys {

using namespace tinyxml2;
namespace fs = boost::filesystem;

class ChainImpl {
public:
  ChainImpl(Chain &self) : self(self) {}

  void initWorld(nlohmann::json &world_state) {
    world_type world = unmarshalWorldState(world_state);

    appbase::app().setWorldId(world.world_id);

    string auth_cert;
    int multiline_size = world.authority_pk.size() - 1;
    for (int i = 0; i <= multiline_size; ++i) {
      auth_cert += world.authority_pk[i];
      if (i != multiline_size)
        auth_cert += "\n";
    }

    appbase::app().setAuthCert(auth_cert);

    self.saveWorld(world);
    self.saveLatestWorldId(world.world_id);
  }

  optional<vector<string>> initChain(nlohmann::json &chain_state) {
    local_chain_type chain = unmarshalChainState(chain_state);

    if (appbase::app().getWorldId() != chain.world_id)
      return {};

    appbase::app().setChainId(chain.chain_id);

    self.saveChain(chain);
    self.saveLatestChainId(chain.chain_id);

    return chain.tracker_addresses;
  }

  world_type unmarshalWorldState(nlohmann::json &state) {
    try {
      world_type world_state;

      world_state.world_id = state["/world/id"_json_pointer];
      world_state.created_time = state["/world/after"_json_pointer];

      world_state.keyc_name = state["/key_currency/name"_json_pointer];
      world_state.keyc_initial_amount = state["/key_currency/initial_amount"_json_pointer];

      world_state.allow_mining = state["/mining_policy/allow_mining"_json_pointer];

      world_state.allow_anonymous_user = state["/user_policy/allow_anonymous_user"_json_pointer];
      world_state.join_fee = state["/user_policy/join_fee"_json_pointer];

      world_state.eden_sig = state["eden_sig"].get<string>();

      world_state.authority_id = state["/authority/id"_json_pointer];
      vector<string> tmp_authority_cert = state["authority"]["cert"].get<vector<string>>();
      world_state.authority_pk = parseCertContent(tmp_authority_cert);

      world_state.creator_id = state["/creator/id"_json_pointer];
      vector<string> tmp_creator_cert = state["creator"]["cert"].get<vector<string>>();
      world_state.creator_pk = parseCertContent(tmp_creator_cert);
      world_state.creator_sig = state["/creator/sig"_json_pointer];

      return world_state;
    } catch (nlohmann::json::parse_error &e) {
      logger::ERROR("Failed to parse world json: {}", e.what());
      throw e;
    }
  }

  local_chain_type unmarshalChainState(nlohmann::json &state) {
    try {
      local_chain_type chain_state;

      chain_state.chain_id = state["/chain/id"_json_pointer];
      chain_state.world_id = state["/chain/world"_json_pointer];
      chain_state.created_time = state["/chain/after"_json_pointer];

      chain_state.allow_custom_contract = state["/policy/allow_custom_contract"_json_pointer];
      chain_state.allow_oracle = state["/policy/allow_oracle"_json_pointer];
      chain_state.allow_tag = state["/policy/allow_tag"_json_pointer];
      chain_state.allow_heavy_contract = state["/policy/allow_heavy_contract"_json_pointer];

      chain_state.creator_id = state["/creator/id"_json_pointer];
      vector<string> tmp_creator_cert = state["creator"]["cert"].get<vector<string>>();
      chain_state.creator_pk = parseCertContent(tmp_creator_cert);
      chain_state.creator_sig = state["/creator/sig"_json_pointer];

      for (auto &tracker_info : state["tracker"]) {
        auto address = tracker_info["address"].get<string>();
        chain_state.tracker_addresses.emplace_back(address);
      }

      return chain_state;
    } catch (nlohmann::json::parse_error &e) {
      logger::ERROR("Failed to parse local chain json: {}", e.what());
      throw e;
    }
  }

  string parseCertContent(std::vector<string> &cert) {
    string cert_content = "";
    for (int i = 0; i < cert.size(); ++i) {
      cert_content += cert[i];
      cert_content += "\n";
    }
    return cert_content;
  }

  Chain &self;
};

void Chain::loadBuiltInContracts(const string &contracts_dir_path) {
  fs::path dir_path(contracts_dir_path);
  if (dir_path.is_relative())
    dir_path = fs::current_path() / dir_path;

  map<string, string> contracts;

  for (auto i = fs::directory_iterator(dir_path); i != fs::directory_iterator(); ++i) {
    if (fs::is_directory(i->path()))
      continue;

    XMLDocument xml_doc;
    xml_doc.LoadFile(i->path().string().data());

    XMLNode *root = xml_doc.FirstChild();
    XMLElement *head = root->FirstChildElement("head");

    string cid = head->FirstChildElement("cid")->GetText();
    string contract_type = cid.substr(0, cid.find("::"));

    tinyxml2::XMLPrinter printer;
    xml_doc.Accept(&printer);

    string raw_xml = printer.CStr();
    contracts[contract_type] = raw_xml;
  }

  saveBuiltInContracts(contracts);
}

void Chain::initBuiltInContracts() {
  auto chain_id = appbase::app().getChainId();
  auto chain_after = getValueByKey(DataType::CHAIN, chain_id + "_ctime");
  auto chain_author = getValueByKey(DataType::CHAIN, chain_id + "_crid");

  auto world_id = appbase::app().getWorldId();
  auto world_creator = getValueByKey(DataType::WORLD, world_id + "_cid");

  map<contract_id_type, contract_type> contract_list;
  // TODO : must handle it differently depending on contract type
  //       currently, only one being handled ( VALUE_TRANSFER )
  for (auto &contract_name : BuiltInContractList) {
    auto raw_xml = getValueByKey(DataType::BUILT_IN_CONTRACT, contract_name);

    XMLDocument xml_doc;
    xml_doc.Parse(raw_xml.data());

    XMLNode *root = xml_doc.FirstChild();
    XMLElement *head = root->FirstChildElement("head");

    string cid = contract_name + "::" + world_creator + "::" + chain_id + "::" + world_id;
    string desc = head->FirstChildElement("desc")->GetText();

    head->FirstChildElement("cid")->SetText(cid.data());
    head->FirstChildElement("after")->SetText(chain_after.data());

    tinyxml2::XMLPrinter printer;
    xml_doc.Accept(&printer);

    contract_type contract;
    contract.cid = cid;
    contract.after = (uint64_t)stol(chain_after);
    contract.desc = desc;
    contract.author = chain_author;
    contract.query_type = QueryType::INSERT;
    contract.contract = printer.CStr();

    contract_list[cid] = contract;
  }

  applyContractToRDB(contract_list);
}

void Chain::initWorld(nlohmann::json &world_state) {
  impl->initWorld(world_state);
}

optional<vector<string>> Chain::initChain(nlohmann::json &chain_state) {
  return impl->initChain(chain_state);
}

Chain::Chain(string_view dbms, string_view table_name, string_view db_user_id, string_view db_password) {
  impl = make_unique<ChainImpl>(*this);
  rdb_controller = make_unique<RdbController>(dbms, table_name, db_user_id, db_password);
  kv_controller = make_unique<KvController>();
  unresolved_block_pool = make_unique<UnresolvedBlockPool>();
  m_us_tree = StateTree();
  m_cs_tree = StateTree();
}

Chain::~Chain() {
  impl.reset();
}

// KV functions
const nlohmann::json Chain::queryWorldGet() {
  world_type current_world = kv_controller->loadCurrentWorld();

  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("world_id");
  result_json["name"].push_back("created_time");
  result_json["name"].push_back("creator_id");
  result_json["name"].push_back("creator_pk");
  result_json["name"].push_back("authority_id");
  result_json["name"].push_back("authority_pk");
  result_json["name"].push_back("keyc_name");
  result_json["name"].push_back("keyc_initial_amount");
  result_json["name"].push_back("allow_mining");
  result_json["name"].push_back("mining_rule");
  result_json["name"].push_back("allow_anonymous_user");
  result_json["name"].push_back("join_fee");

  result_json["data"] = nlohmann::json::array();
  result_json["data"][0] = nlohmann::json::array();
  result_json["data"][0].push_back(current_world.world_id);
  result_json["data"][0].push_back(current_world.created_time);
  result_json["data"][0].push_back(current_world.creator_id);
  result_json["data"][0].push_back(current_world.creator_pk);
  result_json["data"][0].push_back(current_world.authority_id);
  result_json["data"][0].push_back(current_world.authority_pk);
  result_json["data"][0].push_back(current_world.keyc_name);
  result_json["data"][0].push_back(current_world.keyc_initial_amount);
  result_json["data"][0].push_back(current_world.allow_mining);
  result_json["data"][0].push_back(current_world.mining_rule);
  result_json["data"][0].push_back(current_world.allow_anonymous_user);
  result_json["data"][0].push_back(current_world.join_fee);

  return result_json;
}

const nlohmann::json Chain::queryChainGet() {
  local_chain_type current_chain = kv_controller->loadCurrentChain();

  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("chain_id");
  result_json["name"].push_back("created_time");
  result_json["name"].push_back("creator_id");
  result_json["name"].push_back("creator_pk");
  result_json["name"].push_back("allow_custom_contract");
  result_json["name"].push_back("allow_oracle");
  result_json["name"].push_back("allow_tag");
  result_json["name"].push_back("allow_heavy_contract");

  result_json["data"] = nlohmann::json::array();
  result_json["data"][0] = nlohmann::json::array();
  result_json["data"][0].push_back(current_chain.chain_id);
  result_json["data"][0].push_back(current_chain.created_time);
  result_json["data"][0].push_back(current_chain.creator_id);
  result_json["data"][0].push_back(current_chain.creator_pk);
  result_json["data"][0].push_back(current_chain.allow_custom_contract);
  result_json["data"][0].push_back(current_chain.allow_oracle);
  result_json["data"][0].push_back(current_chain.allow_tag);
  result_json["data"][0].push_back(current_chain.allow_heavy_contract);

  return result_json;
}

const nlohmann::json Chain::queryBlockGet(const nlohmann::json &where_json) {
  // TODO: 추후 RDB로 구현
  nlohmann::json block_msg = nlohmann::json::from_cbor(kv_controller->queryBlockGet(where_json));
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("block");

  result_json["data"] = nlohmann::json::array();
  result_json["data"][0] = nlohmann::json::array();
  result_json["data"][0].push_back(block_msg);

  return result_json;
}

// KV functions
void Chain::saveBuiltInContracts(map<string, string> &contracts) {
  kv_controller->saveBuiltInContracts(contracts);
}

void Chain::saveLatestWorldId(const alphanumeric_type &world_id) {
  kv_controller->saveLatestWorldId(world_id);
}

void Chain::saveLatestChainId(const alphanumeric_type &chain_id) {
  kv_controller->saveLatestChainId(chain_id);
}

void Chain::saveWorld(world_type &world_info) {
  kv_controller->saveWorld(world_info);
}

void Chain::saveChain(local_chain_type &chain_info) {
  kv_controller->saveChain(chain_info);
}

void Chain::saveBlockIds() {
  nlohmann::json id_array = unresolved_block_pool->getPoolBlockIds();

  kv_controller->saveBlockIds(TypeConverter::bytesToString(nlohmann::json::to_cbor(id_array)));
}

void Chain::saveBackupBlock(const nlohmann::json &block_json) {
  base58_type block_id = json::get<string>(block_json["block"], "id").value();
  string serialized_block = TypeConverter::toString(nlohmann::json::to_cbor(block_json));

  kv_controller->saveBackupBlock(block_id, serialized_block);
}

void Chain::saveBackupResult(const UnresolvedBlock &UR_block) {
  base58_type block_id = UR_block.block.getBlockId();
  kv_controller->saveBackupUserLedgers(block_id, unresolved_block_pool->serializeUserLedgerList(UR_block));
  kv_controller->saveBackupContractLedgers(block_id, unresolved_block_pool->serializeContractLedgerList(UR_block));
  kv_controller->saveBackupUserAttributes(block_id, unresolved_block_pool->serializeUserAttributeList(UR_block));
  kv_controller->saveBackupUserCerts(block_id, unresolved_block_pool->serializeUserCertList(UR_block));
  kv_controller->saveBackupContracts(block_id, unresolved_block_pool->serializeContractList(UR_block));
}

void Chain::saveSelfInfo(self_info_type &self_info) {
  kv_controller->saveSelfInfo(self_info);
}

string Chain::getValueByKey(string what, const string &base_keys) {
  return kv_controller->getValueByKey(what, base_keys);
}

void Chain::restorePool() {
  // TODO: 예외처리
  string serialized_id_array = kv_controller->loadBlockIds();
  if (serialized_id_array.empty()) {
    return;
  }

  nlohmann::json id_array_json = nlohmann::json::from_cbor(serialized_id_array);
  for (auto &each_block_id : id_array_json) {
    base58_type block_id = each_block_id;

    // TODO: serialized_id_array가 empty string인 경우, from_cbor에서 exception 처리
    // TODO: initialize 가 실패하는 case 처리
    nlohmann::json block_msg = nlohmann::json::from_cbor(kv_controller->loadBackupBlock(block_id));
    Block restored_block;
    restored_block.initialize(block_msg);
    block_push_result_type push_result = unresolved_block_pool->pushBlock(restored_block);

    UnresolvedBlock restored_unresolved_block =
        unresolved_block_pool->getUnresolvedBlock(restored_block.getBlockId(), push_result.block_height);

    nlohmann::json user_ledger_list_json = nlohmann::json::from_cbor(kv_controller->loadBackupUserLedgers(block_id));
    nlohmann::json contract_ledger_list_json = nlohmann::json::from_cbor(kv_controller->loadBackupContractLedgers(block_id));
    nlohmann::json user_attribute_list_json = nlohmann::json::from_cbor(kv_controller->loadBackupUserAttributes(block_id));
    nlohmann::json user_cert_list_json = nlohmann::json::from_cbor(kv_controller->loadBackupUserCerts(block_id));
    nlohmann::json contract_list_json = nlohmann::json::from_cbor(kv_controller->loadBackupContracts(block_id));

    restoreUserLedgerList(restored_unresolved_block, user_ledger_list_json);
    restoreContractLedgerList(restored_unresolved_block, contract_ledger_list_json);
    restoreUserAttributeList(restored_unresolved_block, user_attribute_list_json);
    restoreUserCertList(restored_unresolved_block, user_cert_list_json);
    restoreContractList(restored_unresolved_block, contract_list_json);
  }
}

void Chain::restoreUserLedgerList(UnresolvedBlock &restored_unresolved_block, const nlohmann::json &user_ledger_list_json) {
  for (auto &each_user_ledger_json : user_ledger_list_json) {
    user_ledger_type each_user_ledger;

    each_user_ledger.var_name = json::get<string>(each_user_ledger_json, "var_name").value();
    each_user_ledger.var_value = json::get<string>(each_user_ledger_json, "var_value").value();
    each_user_ledger.var_type = stoi(json::get<string>(each_user_ledger_json, "var_type").value());
    each_user_ledger.uid = json::get<string>(each_user_ledger_json, "uid").value();
    each_user_ledger.up_time = static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_user_ledger_json, "up_time").value()));
    each_user_ledger.up_block = stoi(json::get<string>(each_user_ledger_json, "up_block").value());
    each_user_ledger.tag = json::get<string>(each_user_ledger_json, "tag").value();
    each_user_ledger.pid = json::get<string>(each_user_ledger_json, "pid").value();
    each_user_ledger.query_type = static_cast<tethys::QueryType>(stoi(json::get<string>(each_user_ledger_json, "query_type").value()));
    each_user_ledger.is_empty = json::get<bool>(each_user_ledger_json, "is_empty").value();

    restored_unresolved_block.user_ledger_list[each_user_ledger.pid] = each_user_ledger;
  }
}

void Chain::restoreContractLedgerList(UnresolvedBlock &restored_unresolved_block, const nlohmann::json &contract_ledger_list_json) {
  for (auto &each_contract_ledger_json : contract_ledger_list_json) {
    contract_ledger_type each_contract_ledger;

    each_contract_ledger.var_name = json::get<string>(each_contract_ledger_json, "var_name").value();
    each_contract_ledger.var_value = json::get<string>(each_contract_ledger_json, "var_value").value();
    each_contract_ledger.var_type = stoi(json::get<string>(each_contract_ledger_json, "var_type").value());
    each_contract_ledger.cid = json::get<string>(each_contract_ledger_json, "cid").value();
    each_contract_ledger.up_time = static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_contract_ledger_json, "up_time").value()));
    each_contract_ledger.up_block = stoi(json::get<string>(each_contract_ledger_json, "up_block").value());
    each_contract_ledger.var_info = json::get<string>(each_contract_ledger_json, "var_info").value();
    each_contract_ledger.pid = json::get<string>(each_contract_ledger_json, "pid").value();
    each_contract_ledger.query_type =
        static_cast<tethys::QueryType>(stoi(json::get<string>(each_contract_ledger_json, "query_type").value()));
    each_contract_ledger.is_empty = json::get<bool>(each_contract_ledger_json, "is_empty").value();

    restored_unresolved_block.contract_ledger_list[each_contract_ledger.pid] = each_contract_ledger;
  }
}

void Chain::restoreUserAttributeList(UnresolvedBlock &restored_unresolved_block, const nlohmann::json &user_attribute_list_json) {
  for (auto &each_user_attribute_json : user_attribute_list_json) {
    user_attribute_type each_user_attribute;

    each_user_attribute.uid = json::get<string>(each_user_attribute_json, "uid").value();
    each_user_attribute.register_day =
        static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_user_attribute_json, "register_day").value()));
    each_user_attribute.register_code = json::get<string>(each_user_attribute_json, "register_code").value();
    each_user_attribute.gender = stoi(json::get<string>(each_user_attribute_json, "gender").value());
    each_user_attribute.isc_type = json::get<string>(each_user_attribute_json, "isc_type").value();
    each_user_attribute.isc_code = json::get<string>(each_user_attribute_json, "isc_code").value();
    each_user_attribute.location = json::get<string>(each_user_attribute_json, "location").value();
    each_user_attribute.age_limit = stoi(json::get<string>(each_user_attribute_json, "age_limit").value());
    each_user_attribute.sigma = json::get<string>(each_user_attribute_json, "sigma").value();

    restored_unresolved_block.user_attribute_list[each_user_attribute.uid] = each_user_attribute;
  }
}

void Chain::restoreUserCertList(UnresolvedBlock &restored_unresolved_block, const nlohmann::json &user_cert_list_json) {
  for (auto &each_user_cert_json : user_cert_list_json) {
    user_cert_type each_user_cert;

    each_user_cert.uid = json::get<string>(each_user_cert_json, "uid").value();
    each_user_cert.sn = json::get<string>(each_user_cert_json, "sn").value();
    each_user_cert.nvbefore = static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_user_cert_json, "nvbefore").value()));
    each_user_cert.nvafter = static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_user_cert_json, "nvafter").value()));
    each_user_cert.x509 = json::get<string>(each_user_cert_json, "x509").value();

    restored_unresolved_block.user_cert_list[each_user_cert.sn] = each_user_cert;
  }
}

void Chain::restoreContractList(UnresolvedBlock &restored_unresolved_block, const nlohmann::json &contract_list_json) {
  for (auto &each_contract_json : contract_list_json) {
    contract_type each_contract;

    each_contract.cid = json::get<string>(each_contract_json, "cid").value();
    each_contract.after = static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_contract_json, "after").value()));
    each_contract.before = static_cast<tethys::timestamp_t>(stoll(json::get<string>(each_contract_json, "before").value()));
    each_contract.author = json::get<string>(each_contract_json, "author").value();
    each_contract.friends = json::get<string>(each_contract_json, "friends").value();
    each_contract.contract = json::get<string>(each_contract_json, "contract").value();
    each_contract.desc = json::get<string>(each_contract_json, "desc").value();
    each_contract.sigma = json::get<string>(each_contract_json, "sigma").value();

    restored_unresolved_block.contract_list[each_contract.cid] = each_contract;
  }
}

// RDB functions
const nlohmann::json Chain::queryContractScan(const nlohmann::json &where_json) {
  vector<contract_id_type> found_contracts = rdb_controller->queryContractScan(where_json);

  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("cid");

  result_json["data"] = nlohmann::json::array();
  for (int i = 0; i < found_contracts.size(); ++i) {
    result_json["data"][i].push_back(found_contracts[i]);
  }

  return result_json;
}

const nlohmann::json Chain::queryContractGet(const nlohmann::json &where_json) {
  string found_contract = rdb_controller->queryContractGet(where_json);

  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("contract");

  result_json["data"] = nlohmann::json::array();
  result_json["data"][0].push_back(found_contract);

  return result_json;
}

const nlohmann::json Chain::queryCertGet(const nlohmann::json &where_json) {
  vector<user_cert_type> found_certs = rdb_controller->queryCertGet(where_json);

  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("sn");
  result_json["name"].push_back("nvbefore");
  result_json["name"].push_back("nvafter");
  result_json["name"].push_back("x509");

  result_json["data"] = nlohmann::json::array();
  for (int i = 0; i < found_certs.size(); ++i) {
    result_json["data"][i].push_back(found_certs[i].sn);
    result_json["data"][i].push_back(to_string(found_certs[i].nvbefore));
    result_json["data"][i].push_back(to_string(found_certs[i].nvafter));
    result_json["data"][i].push_back(found_certs[i].x509);
  }

  return result_json;
}

const nlohmann::json Chain::queryUserInfoGet(const nlohmann::json &where_json) {
  user_attribute_type found_user_info = rdb_controller->queryUserInfoGet(where_json);
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("register_day");
  result_json["name"].push_back("register_code");
  result_json["name"].push_back("gender");
  result_json["name"].push_back("isc_type");
  result_json["name"].push_back("isc_code");
  result_json["name"].push_back("location");
  result_json["name"].push_back("age_limit");

  result_json["data"] = nlohmann::json::array();
  result_json["data"][0].push_back(to_string(found_user_info.register_day));
  result_json["data"][0].push_back(found_user_info.register_code);
  result_json["data"][0].push_back(to_string(found_user_info.gender));
  result_json["data"][0].push_back(found_user_info.isc_type);
  result_json["data"][0].push_back(found_user_info.isc_code);
  result_json["data"][0].push_back(found_user_info.location);
  result_json["data"][0].push_back(to_string(found_user_info.age_limit));

  return result_json;
}

const nlohmann::json Chain::queryUserScopeGet(const nlohmann::json &where_json) {
  vector<user_ledger_type> found_user_ledgers = rdb_controller->queryUserScopeGet(where_json);
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("var_name");
  result_json["name"].push_back("var_value");
  result_json["name"].push_back("var_type");
  result_json["name"].push_back("up_time");
  result_json["name"].push_back("up_block");
  result_json["name"].push_back("tag");
  result_json["name"].push_back("pid");

  result_json["data"] = nlohmann::json::array();
  for (int i = 0; i < found_user_ledgers.size(); ++i) {
    result_json["data"][i].push_back(found_user_ledgers[i].var_name);
    result_json["data"][i].push_back(found_user_ledgers[i].var_value);
    result_json["data"][i].push_back(to_string(found_user_ledgers[i].var_type));
    result_json["data"][i].push_back(to_string(found_user_ledgers[i].up_time));
    result_json["data"][i].push_back(to_string(found_user_ledgers[i].up_block));
    result_json["data"][i].push_back(found_user_ledgers[i].tag);
    result_json["data"][i].push_back(found_user_ledgers[i].pid);
  }

  return result_json;
}

const nlohmann::json Chain::queryContractScopeGet(const nlohmann::json &where_json) {
  vector<contract_ledger_type> found_contract_ledgers = rdb_controller->queryContractScopeGet(where_json);
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("var_name");
  result_json["name"].push_back("var_value");
  result_json["name"].push_back("var_type");
  result_json["name"].push_back("var_info");
  result_json["name"].push_back("up_time");
  result_json["name"].push_back("up_block");
  result_json["name"].push_back("pid");

  result_json["data"] = nlohmann::json::array();
  for (int i = 0; i < found_contract_ledgers.size(); ++i) {
    result_json["data"][i].push_back(found_contract_ledgers[i].var_name);
    result_json["data"][i].push_back(found_contract_ledgers[i].var_value);
    result_json["data"][i].push_back(found_contract_ledgers[i].var_type);
    result_json["data"][i].push_back(found_contract_ledgers[i].var_info);
    result_json["data"][i].push_back(to_string(found_contract_ledgers[i].up_time));
    result_json["data"][i].push_back(to_string(found_contract_ledgers[i].up_block));
    result_json["data"][i].push_back(found_contract_ledgers[i].pid);
  }

  return result_json;
}

// const nlohmann::json Chain::queryBlockGet(const nlohmann::json &where_json) {
//  Block found_block = rdb_controller->queryBlockGet(where_json);
//  nlohmann::json result_json;
//  result_json["name"] = nlohmann::json::array();
//  result_json["name"].push_back("block");
//
//  // TODO: 추후 RDB 구현
//  // TODO: MSG_BLOCK 형태로 return
//  result_json["data"] = nlohmann::json::array();
//
//  return result_json;
//}

const nlohmann::json Chain::queryTxGet(const nlohmann::json &where_json) {
  nlohmann::json msg_tx_agg = nlohmann::json::from_cbor(rdb_controller->queryTxGet(where_json));
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("tx");

  result_json["data"] = nlohmann::json::array();
  result_json["data"][0] = nlohmann::json::array();
  result_json["data"][0].push_back(msg_tx_agg);

  return result_json;
}

const nlohmann::json Chain::queryBlockScan(const nlohmann::json &where_json) {
  vector<base58_type> found_block_ids = rdb_controller->queryBlockScan(where_json);
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("block_id");

  result_json["data"] = nlohmann::json::array();
  for (int i = 0; i < found_block_ids.size(); ++i) {
    result_json["data"][i].push_back(found_block_ids[i]);
  }

  return result_json;
}

const nlohmann::json Chain::queryTxScan(const nlohmann::json &where_json) {
  vector<base58_type> found_tx_ids = rdb_controller->queryTxScan(where_json);
  nlohmann::json result_json;
  result_json["name"] = nlohmann::json::array();
  result_json["name"].push_back("tx_id");

  result_json["data"] = nlohmann::json::array();
  for (int i = 0; i < found_tx_ids.size(); ++i) {
    result_json["data"][i].push_back(found_tx_ids[i]);
  }

  return result_json;
}

string Chain::getUserCert(const base58_type &user_id) {
  return rdb_controller->getUserCert(user_id);
}

bool Chain::applyBlockToRDB(const tethys::Block &block_info) {
  return rdb_controller->applyBlockToRDB(block_info);
}

bool Chain::applyTransactionToRDB(const tethys::Block &block_info) {
  return rdb_controller->applyTransactionToRDB(block_info);
}

bool Chain::applyUserLedgerToRDB(const std::map<string, user_ledger_type> &user_ledger_list) {
  return rdb_controller->applyUserLedgerToRDB(user_ledger_list);
}

bool Chain::applyContractLedgerToRDB(const std::map<string, contract_ledger_type> &contract_ledger_list) {
  return rdb_controller->applyContractLedgerToRDB(contract_ledger_list);
}

bool Chain::applyUserAttributeToRDB(const std::map<base58_type, user_attribute_type> &user_attribute_list) {
  return rdb_controller->applyUserAttributeToRDB(user_attribute_list);
}

bool Chain::applyUserCertToRDB(const std::map<base58_type, user_cert_type> &user_cert_list) {
  return rdb_controller->applyUserCertToRDB(user_cert_list);
}

bool Chain::applyContractToRDB(const std::map<base58_type, contract_type> &contract_list) {
  return rdb_controller->applyContractToRDB(contract_list);
}

vector<Block> Chain::getBlocksByHeight(int from, int to) {
  if (from > to) {
    return vector<Block>();
  }
  vector<Block> blocks = rdb_controller->getBlocks(from, to);
  return blocks;
}

block_height_type Chain::getLatestResolvedHeight() {
  auto block = rdb_controller->getLatestResolvedBlock();

  if (block.has_value()) {
    return block.value().getHeight();
  } else {
    return 0;
  }
}

// Unresolved block pool functions
bool Chain::queryUserJoin(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  user_attribute_type user_info;

  user_info.uid = result_info.user;
  user_info.register_day = static_cast<tethys::timestamp_t>(stoll(json::get<string>(option, "register_day").value()));
  user_info.register_code = json::get<string>(option, "register_code").value();
  user_info.gender = stoi(json::get<string>(option, "gender").value());
  user_info.isc_type = json::get<string>(option, "isc_type").value();
  user_info.isc_code = json::get<string>(option, "isc_code").value();
  user_info.location = json::get<string>(option, "location").value();
  user_info.age_limit = stoi(json::get<string>(option, "age_limit").value());

  string msg = user_info.uid + to_string(user_info.register_day) + user_info.register_code + to_string(user_info.gender) +
               user_info.isc_type + user_info.isc_code + user_info.location + to_string(user_info.age_limit);

  // TODO: signature by tethys Authority 정확히 구현
  AGS ags;
  /// auto sigma = ags.sign(TethysAuthority_secret_key, msg);
  user_info.sigma = "TODO: signature by Tethys Authority";

  UR_block.user_attribute_list[user_info.uid] = user_info;

  return true;
}

bool Chain::queryUserCert(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  // TODO: user cert에는 한 사람이 유효기간이 서로 다른 인증서 여러 개가 저장될 수 있음
  user_cert_type user_cert;

  user_cert.uid = result_info.user;
  user_cert.sn = json::get<string>(option, "sn").value();
  user_cert.nvbefore = static_cast<uint64_t>(stoll(json::get<string>(option, "notbefore").value()));
  user_cert.nvafter = static_cast<uint64_t>(stoll(json::get<string>(option, "notafter").value()));
  user_cert.x509 = json::get<string>(option, "x509").value();

  UR_block.user_cert_list[user_cert.sn] = user_cert;

  return true;
}

bool Chain::queryContractNew(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  contract_type contract_info;

  contract_info.cid = json::get<string>(option, "cid").value();
  contract_info.after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));
  contract_info.before = static_cast<uint64_t>(stoll(json::get<string>(option, "before").value()));
  contract_info.author = json::get<string>(option, "author").value();

  // TODO: friend 추가할 때 자신을 friend로 추가한 contract를 찾아서 추가해야 함
  contract_id_type friends = json::get<string>(option, "friend").value();

  contract_info.contract = json::get<string>(option, "contract").value();
  contract_info.desc = json::get<string>(option, "desc").value();
  contract_info.sigma = json::get<string>(option, "sigma").value();
  contract_info.query_type = QueryType::INSERT;

  UR_block.contract_list[contract_info.cid] = contract_info;

  return true;
}

bool Chain::queryContractDisable(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  // TODO: $user = cid.author인 경우에만 허용
  contract_type contract_info;

  contract_info.cid = json::get<string>(option, "cid").value();
  contract_info.before = TimeUtil::nowBigInt();
  contract_info.query_type = QueryType::UPDATE;

  // TODO: 수정 필요. 동일 블럭에 있던 contract일 경우 다른 정보가 증발하고 before만 현재시각으로 기록될 수 있음. UPDATE라 에러가 나거나.
  //  무력화되는 contract이기 때문에 동작 자체에 문제를 일으키지는 않겠지만, 깔끔한 작동은 아님.

  UR_block.contract_list[contract_info.cid] = contract_info;

  return true;
}

bool Chain::queryIncinerate(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  string amount = json::get<string>(option, "amount").value();
  string pid = json::get<string>(option, "pid").value();

  user_ledger_type found_ledger = findUserLedgerFromHead(UR_block, pid);

  if (found_ledger.is_empty)
    return false;

  if (found_ledger.uid != result_info.user)
    return false;

  int modified_value = stoi(found_ledger.var_value) - stoi(amount);
  found_ledger.var_value = to_string(modified_value);

  if (modified_value == 0)
    found_ledger.query_type = QueryType::DELETE;
  else if (modified_value > 0)
    found_ledger.query_type = QueryType::UPDATE;
  else
    logger::ERROR("var_value is under 0 in queryIncinerate!");

  UR_block.user_ledger_list[pid] = found_ledger;

  return true;
}

bool Chain::queryCreate(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  string amount = json::get<string>(option, "amount").value();
  string var_name = json::get<string>(option, "name").value();
  int var_type = stoi(json::get<string>(option, "type").value());
  string tag = json::get<string>(option, "tag").value();
  base58_type uid = result_info.user;
  timestamp_t up_time = TimeUtil::nowBigInt(); // TODO: DB에 저장되는 시간인지, mem_ledger에 들어오는 시간인지
  block_height_type up_block = result_info.block_height;

  user_ledger_type user_ledger(var_name, var_type, uid, tag);
  string pid = user_ledger.pid;

  user_ledger_type found = findUserLedgerFromHead(UR_block, pid);

  if (!found.is_empty) {
    logger::ERROR("Same pid state is exist. v.create must be new.");
    return false;
  }

  user_ledger.var_value = amount;
  user_ledger.up_time = up_time;
  user_ledger.up_block = up_block;
  user_ledger.query_type = QueryType::INSERT;
  user_ledger.is_empty = false;

  UR_block.user_ledger_list[pid] = user_ledger;

  return true;
}

bool Chain::queryTransfer(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  // TODO: authority 검사
  //  up_time, up_block 어떻게 설정할지 검토
  string from = json::get<string>(option, "from").value();
  string from_id;
  string to_id = json::get<string>(option, "to").value();
  string amount = json::get<string>(option, "amount").value();
  string var_name = json::get<string>(option, "unit").value();
  timestamp_t up_time = TimeUtil::nowBigInt(); // TODO: DB에 저장되는 시간인지, mem_ledger에 들어오는 시간인지
  block_height_type up_block = result_info.block_height;
  auto from_pid_json = json::get<string>(option, "pid");
  string from_pid;
  string to_pid;
  auto tag_json = json::get<string>(option, "tag");
  string tag;
  int var_type;

  // Transfer : from
  if (from == "contract") {
    // from : contract
    from_id = result_info.self;

    if (from_pid_json.has_value()) {
      from_pid = from_pid_json.value();
    } else {
      var_type = getVarType(from_id, var_name, UR_block.block.getHeight(), UR_block.cur_vec_idx);
      if (var_type == (int)UniqueCheck::NOT_UNIQUE) {
        logger::ERROR("Error in `queryTransfer`, there was several same (var owner, var name).");
        return false;
      }
      from_pid = calculatePid(var_name, var_type, from_id);
    }

    contract_ledger_type found_ledger = findContractLedgerFromHead(UR_block, from_pid);

    if (found_ledger.is_empty) {
      logger::ERROR("Not exist from's ledger");
      return false;
    }

    int modified_value = stoi(found_ledger.var_value) - stoi(amount);
    found_ledger.var_value = to_string(modified_value);
    found_ledger.up_time = up_time;
    found_ledger.up_block = up_block;

    if (modified_value == 0)
      found_ledger.query_type = QueryType::DELETE;
    else if (modified_value > 0)
      found_ledger.query_type = QueryType::UPDATE;
    else {
      logger::ERROR("var_value is under 0 in queryTransfer!");
      return false;
    }

    UR_block.contract_ledger_list[from_pid] = found_ledger;
    var_type = found_ledger.var_type;
  } else {
    // from : user
    if (from == "user")
      from_id = result_info.user;
    else if (from == "author")
      from_id = result_info.author;

    if (from_pid_json.has_value()) {
      from_pid = from_pid_json.value();
    } else {
      var_type = getVarType(from_id, var_name, UR_block.block.getHeight(), UR_block.cur_vec_idx);

      if (var_type == (int)UniqueCheck::NOT_UNIQUE) {
        logger::ERROR("Error in `queryTransfer`, there was several same (var owner, var name).");
        return false;
      }
      from_pid = calculatePid(var_name, var_type, from_id);
    }

    user_ledger_type found_ledger = findUserLedgerFromHead(UR_block, from_pid);

    if (found_ledger.is_empty) {
      logger::ERROR("Not exist from's ledger");
      return false;
    }

    int modified_value = stoi(found_ledger.var_value) - stoi(amount);
    found_ledger.var_value = to_string(modified_value);
    found_ledger.up_time = up_time;
    found_ledger.up_block = up_block;

    if (modified_value == 0)
      found_ledger.query_type = QueryType::DELETE;
    else if (modified_value > 0)
      found_ledger.query_type = QueryType::UPDATE;
    else {
      logger::ERROR("var_value is under 0 in queryTransfer!");
      return false;
    }

    UR_block.user_ledger_list[from_pid] = found_ledger;
    var_type = found_ledger.var_type;
  }

  // Transfer : to
  if (isContractId(to_id)) {
    // to : contract
    to_pid = calculatePid(var_name, var_type, to_id, from_id);

    contract_ledger_type found_ledger = findContractLedgerFromHead(UR_block, to_pid);

    if (found_ledger.var_value.empty() || found_ledger.is_empty)
      found_ledger.query_type = QueryType::INSERT;
    else if ((found_ledger.up_block == up_block) && (found_ledger.query_type == QueryType::INSERT)) {
      found_ledger.query_type = QueryType::INSERT;
    } else {
      found_ledger.query_type = QueryType::UPDATE;
    }

    int modified_value = stoi(found_ledger.var_value) + stoi(amount);
    found_ledger.var_value = to_string(modified_value);
    found_ledger.up_time = up_time;
    found_ledger.up_block = up_block;
    found_ledger.is_empty = false;

    UR_block.contract_ledger_list[to_pid] = found_ledger;
  } else if (isUserId(to_id)) {
    // to : user
    if (tag_json.has_value()) {
      tag = tag_json.value();
      to_pid = calculatePid(var_name, var_type, to_id, tag);
    } else {
      to_pid = calculatePid(var_name, var_type, to_id);
    }

    user_ledger_type found_ledger = findUserLedgerFromHead(UR_block, to_pid);

    if (found_ledger.var_value.empty() || found_ledger.is_empty)
      found_ledger.query_type = QueryType::INSERT;
    else if ((found_ledger.up_block == up_block) && (found_ledger.query_type == QueryType::INSERT)) {
      found_ledger.query_type = QueryType::INSERT;
    } else {
      found_ledger.query_type = QueryType::UPDATE;
    }

    int modified_value = stoi(found_ledger.var_value) + stoi(amount);
    found_ledger.var_value = to_string(modified_value);
    found_ledger.up_time = up_time;
    found_ledger.up_block = up_block;
    found_ledger.is_empty = false;

    UR_block.user_ledger_list[to_pid] = found_ledger;
  }

  return true;
}

bool Chain::queryUserScope(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  // TODO: authority 검사
  string var_name;
  base58_type uid;
  string var_val;
  string calculated_pid;
  string tag;
  int var_type;
  QueryType query_type;

  var_name = json::get<string>(option, "name").value();

  string target = json::get<string>(option, "for").value(); // user or author
  if (target == "user") {
    uid = result_info.user;
  } else if (target == "author") {
    uid = result_info.author;
  } else {
    logger::ERROR("target is not 'user' or 'author' at `queryUserScope`");
  }

  var_val = json::get<string>(option, "value").value();

  auto pid_json = json::get<string>(option, "pid");
  auto tag_json = json::get<string>(option, "tag");
  auto type_json = json::get<string>(option, "type");

  if (pid_json.has_value()) {
    if (tag_json.has_value()) {
      tag = tag_json.value();
    }

    if (type_json.has_value()) {
      query_type = QueryType::INSERT;
      var_type = stoi(type_json.value());
    } else {
      query_type = QueryType::UPDATE;
    }

    calculated_pid = pid_json.value();

  } else { // pid가 존재하지 않으므로, tag가 없는 항목에만 접근할 수 있다
    if (tag_json.has_value()) {
      logger::ERROR("To use the tag, the pid must be given.");
    }

    if (type_json.has_value()) {
      query_type = QueryType::INSERT;
      var_type = stoi(type_json.value());
    } else {
      query_type = QueryType::UPDATE;
      var_type = getVarType(uid, var_name, UR_block.block.getHeight(), UR_block.cur_vec_idx);

      if (var_type == (int)UniqueCheck::NOT_UNIQUE) {
        logger::ERROR("Error in `queryUserScope`, there was several same (var owner, var name).");
        return false;
      }
    }

    calculated_pid = calculatePid(var_name, var_type, uid);
  }

  // TODO: '통화'속성의 변수는 변경 불가능한 조건 검사 시행

  user_ledger_type user_ledger;

  user_ledger.var_name = var_name;
  user_ledger.var_value = var_val;
  user_ledger.var_type = var_type;
  user_ledger.uid = uid;
  user_ledger.up_time = TimeUtil::nowBigInt(); // TODO: DB에 저장되는 시간인지, mem_ledger에 들어오는 시간인지
  user_ledger.up_block = UR_block.block.getHeight();
  user_ledger.tag = tag;
  user_ledger.pid = calculated_pid;
  user_ledger.query_type = query_type;
  user_ledger.is_empty = false;

  UR_block.user_ledger_list[calculated_pid] = user_ledger;

  return true;
}

bool Chain::queryContractScope(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  // TODO: friend나 self가 아니면 변수를 업데이트할 수 없음
  string var_name;
  string cid;
  string var_val;
  string calculated_pid;
  string var_info;
  int var_type;
  QueryType query_type;

  var_name = json::get<string>(option, "name").value();
  cid = json::get<string>(option, "cid").value();
  var_val = json::get<string>(option, "value").value();

  auto pid_json = json::get<string>(option, "pid");
  auto type_json = json::get<string>(option, "type");

  if (pid_json.has_value()) {
    if (type_json.has_value()) {
      query_type = QueryType::INSERT;
      var_type = stoi(type_json.value());
    } else {
      query_type = QueryType::UPDATE;
    }

    calculated_pid = pid_json.value();

  } else {
    if (type_json.has_value()) {
      query_type = QueryType::INSERT;
      var_type = stoi(type_json.value());
    } else {
      query_type = QueryType::UPDATE;
      var_type = getVarType(cid, var_name, UR_block.block.getHeight(), UR_block.cur_vec_idx);

      if (var_type == (int)UniqueCheck::NOT_UNIQUE) {
        logger::ERROR("Error in `queryContractScope`, there was several same (var owner, var name).");
        return false;
      }
    }

    calculated_pid = calculatePid(var_name, var_type, cid);
  }

  // TODO: '통화'속성의 변수는 변경 불가능한 조건 검사 시행

  contract_ledger_type contract_ledger;

  contract_ledger.var_name = var_name;
  contract_ledger.var_value = var_val;
  contract_ledger.var_type = var_type;
  contract_ledger.cid = cid;
  contract_ledger.up_time = TimeUtil::nowBigInt(); // TODO: DB에 저장되는 시간인지, mem_ledger에 들어오는 시간인지
  contract_ledger.up_block = UR_block.block.getHeight();
  contract_ledger.var_info = var_info;
  contract_ledger.pid = calculated_pid;
  contract_ledger.query_type = query_type;
  contract_ledger.is_empty = false;

  UR_block.contract_ledger_list[calculated_pid] = contract_ledger;

  return true;
}

bool Chain::queryTradeItem(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  nlohmann::json costs = option["costs"];
  nlohmann::json units = option["units"];
  base58_type author = json::get<string>(option, "author").value();
  base58_type user = json::get<string>(option, "user").value();
  string pid = json::get<string>(option, "pid").value();

  // TODO: 처리 절차 구현할 것
  return true;
}

bool Chain::queryTradeVal(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  // TODO: TBA (19.05.21 현재 문서에 관련 사항 미기재)
  return true;
}

bool Chain::queryRunQuery(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  string type = json::get<string>(option, "type").value();
  nlohmann::json query = option["query"];
  timestamp_t after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));

  if ((type == "run.query") || (type == "user.cert")) {
    logger::ERROR("run.query cannot excute 'run.query' or 'user.cert'!");
    return false;
  }
  // TODO: Scheduler에게 지연 처리 요청 전송
  return true;
}

bool Chain::queryRunContract(UnresolvedBlock &UR_block, nlohmann::json &option, result_query_info_type &result_info) {
  contract_id_type cid = json::get<string>(option, "cid").value();
  string input = json::get<string>(option, "input").value();
  timestamp_t after = static_cast<uint64_t>(stoll(json::get<string>(option, "after").value()));

  // TODO: authority.user를 현재 user로 대체하여 Scheduler에게 요청 전송
  return true;
}

string Chain::calculatePid(const string &var_name, int var_type, const string &var_owner) {
  string calculated_pid;

  BytesBuilder bytes_builder;
  bytes_builder.append(var_name);
  bytes_builder.appendDec(var_type);
  if (isContractId(var_owner))
    bytes_builder.append(var_owner);
  else if (isUserId(var_owner))
    bytes_builder.appendBase<58>(var_owner);
  calculated_pid = TypeConverter::bytesToString(Sha256::hash(bytes_builder.getBytes()));

  return calculated_pid;
}

string Chain::calculatePid(const string &var_name, int var_type, const string &var_owner, const string &tag_varinfo) {
  string calculated_pid;

  BytesBuilder bytes_builder;
  bytes_builder.append(var_name);
  bytes_builder.appendDec(var_type);
  if (isContractId(var_owner))
    bytes_builder.append(var_owner);
  else if (isUserId(var_owner))
    bytes_builder.appendBase<58>(var_owner);
  bytes_builder.append(tag_varinfo);
  calculated_pid = TypeConverter::bytesToString(Sha256::hash(bytes_builder.getBytes()));

  return calculated_pid;
}

int Chain::getVarType(const string &var_owner, const string &var_name, const block_height_type height, const int vec_idx) {
  int var_type = (int)UniqueCheck::NO_VALUE;
  int pool_deque_idx = height - unresolved_block_pool->getLatestConfirmedHeight() - 1;
  int pool_vec_idx = vec_idx;

  if (isUserId(var_owner)) {
    map<string, user_ledger_type> current_user_ledgers;

    // unresolved block pool의 연결된 앞 블록들에서 검색
    while (pool_deque_idx >= 0) {
      current_user_ledgers = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).user_ledger_list;
      for (auto &each_ledger : current_user_ledgers) {
        if ((each_ledger.second.uid == var_owner) && (each_ledger.second.var_name == var_name) && (each_ledger.second.tag.empty())) {
          if ((var_type != (int)UniqueCheck::NO_VALUE) && (var_type != each_ledger.second.var_type)) {
            return (int)UniqueCheck::NOT_UNIQUE; // var_type가 초기값이 아닌데 새로운 값이 오면 unique하지 않다는 것
          }
          var_type = each_ledger.second.var_type;
        }
      }
      pool_vec_idx = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).prev_vec_idx;
      --pool_deque_idx;
    }
  } else if (isContractId(var_owner)) {
    map<string, contract_ledger_type> current_contract_ledgers;

    // unresolved block pool의 연결된 앞 블록들에서 검색
    while (pool_deque_idx >= 0) {
      current_contract_ledgers = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).contract_ledger_list;
      for (auto &each_ledger : current_contract_ledgers) { // TODO: var_info가 empty인걸 찾는게 맞는지 확인
        if ((each_ledger.second.cid == var_owner) && (each_ledger.second.var_name == var_name) && (each_ledger.second.var_info.empty())) {
          if ((var_type != (int)UniqueCheck::NO_VALUE) && (var_type != each_ledger.second.var_type)) {
            return (int)UniqueCheck::NOT_UNIQUE; // var_type가 초기값이 아닌데 새로운 값이 오면 unique하지 않다는 것
          }
          var_type = each_ledger.second.var_type;
        }
      }
      pool_vec_idx = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).prev_vec_idx;
      --pool_deque_idx;
    }
  }

  // rdb에서 검색
  int rdb_var_type = rdb_controller->getVarTypeFromRDB(var_owner, var_name);

  if (rdb_var_type == (int)UniqueCheck::NOT_UNIQUE)
    return (int)UniqueCheck::NOT_UNIQUE;

  if ((var_type == (int)UniqueCheck::NO_VALUE) && (rdb_var_type == (int)UniqueCheck::NO_VALUE)) {
    return (int)UniqueCheck::NO_VALUE;
  } else if ((var_type >= 0) && (rdb_var_type >= 0)) { // pool에서도, rdb에서도 값이 하나씩 발견
    if (var_type == rdb_var_type)
      return var_type;
    else
      return (int)UniqueCheck::NOT_UNIQUE;
  } else { // pool과 rdb에서 하나는 NO_VALUE, 하나는 하나의 값
    if (var_type == (int)UniqueCheck::NO_VALUE)
      return rdb_var_type;
    else if (rdb_var_type == (int)UniqueCheck::NO_VALUE)
      return var_type;
  }

  logger::ERROR("Something error in `getVarType`. Cannot reach here.");
}

bool Chain::checkUniqueVarName(const string &var_owner, const string &var_name, const block_height_type height, const int vec_idx) {
  return (getVarType(var_owner, var_name, height, vec_idx) != (int)UniqueCheck::NOT_UNIQUE);
}

block_push_result_type Chain::pushBlock(Block &new_block) {
  block_push_result_type ret_val = unresolved_block_pool->pushBlock(new_block);

  if (m_longest_chain_info.block_height < ret_val.block_height) {
    m_longest_chain_info.block_id = ret_val.block_id;
    m_longest_chain_info.block_height = ret_val.block_height;
    m_longest_chain_info.deq_idx = ret_val.deq_idx;
    m_longest_chain_info.vec_idx = ret_val.vec_idx;
  }
  return ret_val;
}

UnresolvedBlock Chain::getUnresolvedBlock(const base58_type &block_id, const block_height_type block_height) {
  return unresolved_block_pool->getUnresolvedBlock(block_id, block_height);
}

void Chain::setUnresolvedBlock(const UnresolvedBlock &unresolved_block) {
  unresolved_block_pool->setUnresolvedBlock(unresolved_block);
}

bool Chain::resolveBlock(Block &new_block, UnresolvedBlock &resolved_result) {
  vector<base58_type> dropped_block_ids;
  if (!unresolved_block_pool->resolveBlock(new_block, resolved_result, dropped_block_ids))
    return false;

  for (auto &each_block_id : dropped_block_ids) {
    kv_controller->delBackup(each_block_id);
  }

  return true;
}

void Chain::setPool(const base64_type &last_block_id, block_height_type last_height, timestamp_t last_time, const base64_type &last_hash,
                    const base64_type &prev_block_id) {
  return unresolved_block_pool->setPool(last_block_id, last_height, last_time, last_hash, prev_block_id);
}

search_result_type Chain::findUserLedgerFromPoint(const string &pid, block_height_type height, int vec_idx) {
  search_result_type search_result;
  int pool_deque_idx = height - unresolved_block_pool->getLatestConfirmedHeight() - 1;
  int pool_vec_idx = vec_idx;

  map<string, user_ledger_type> current_user_ledgers;
  map<string, user_ledger_type>::iterator it;

  while (1) {
    current_user_ledgers = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).user_ledger_list;

    it = current_user_ledgers.find(pid);
    if (it != current_user_ledgers.end()) {
      search_result.user_ledger = it->second;
      return search_result;
    }
    pool_vec_idx = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).prev_vec_idx;
    --pool_deque_idx;

    if (pool_deque_idx < 0) {
      // -1이 되면 rdb에서 찾을 차례. select문으로 조회하는데도 찾지 못한다면 존재하지 않는 데이터
      search_result.user_ledger = rdb_controller->findUserScopeFromRDB(pid);
      if (search_result.user_ledger.is_empty) {
        search_result.not_found = true;
      }
      return search_result;
    }
  }
}

search_result_type Chain::findContractLedgerFromPoint(const string &pid, block_height_type height, int vec_idx) {
  search_result_type search_result;
  int pool_deque_idx = height - unresolved_block_pool->getLatestConfirmedHeight() - 1;
  int pool_vec_idx = vec_idx;

  map<string, contract_ledger_type> current_contract_ledgers;
  map<string, contract_ledger_type>::iterator it;

  while (1) {
    current_contract_ledgers = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).contract_ledger_list;

    it = current_contract_ledgers.find(pid);
    if (it != current_contract_ledgers.end()) {
      search_result.contract_ledger = it->second;
      return search_result;
    }
    pool_vec_idx = unresolved_block_pool->getUnresolvedBlock(pool_deque_idx, pool_vec_idx).prev_vec_idx;
    --pool_deque_idx;

    if (pool_deque_idx < 0) {
      // -1이 되면 rdb에서 찾을 차례. select문으로 조회하는데도 찾지 못한다면 존재하지 않는 데이터
      search_result.contract_ledger = rdb_controller->findContractScopeFromRDB(pid);
      if (search_result.contract_ledger.is_empty) {
        search_result.not_found = true;
      }
      return search_result;
    }
  }
}

vector<Block> Chain::getBlocksFromUnresolvedLongestChain() {
  auto latest_height = m_longest_chain_info.block_height;
  auto latest_block_id = m_longest_chain_info.block_id;

  auto chain_path = unresolved_block_pool->getPath(latest_block_id, latest_height);

  vector<Block> blocks;
  for (int i = 0; i < chain_path.size(); i++) {
    int deq_idx = i;
    int vec_idx = chain_path[i];
    auto unresolved_block = unresolved_block_pool->getUnresolvedBlock(deq_idx, vec_idx);

    blocks.push_back(unresolved_block.block);
  }

  return blocks;
}

Block &Chain::getLowestUnprocessedBlock() {
  // 가장 긴 chain에서의 처리되지 않은 블록 중 가장 낮은 height block을 return
  return unresolved_block_pool->getLowestUnprocessedBlock(m_longest_chain_info);
}

bool Chain::isUserId(const string &id) {
  const auto BASE58_REGEX = "^[A-HJ-NP-Za-km-z1-9]*$";
  regex rgx(BASE58_REGEX);
  if (id.size() != static_cast<int>(44) || !regex_match(id, rgx)) {
    logger::ERROR("Invalid user ID.");
    return false;
  }
  return true;
}

bool Chain::isContractId(const string &id) {
  // TODO: 검증 추가
  if (id.size() > 45)
    return true;
  else
    return false;
}

// State Tree
void Chain::setupStateTree() {
  m_us_tree.updateUserState(rdb_controller->getAllUserLedger());
  m_cs_tree.updateContractState(rdb_controller->getAllContractLedger());
}

void Chain::updateStateTree(const UnresolvedBlock &unresolved_block) {
  // TODO: 현재의 state tree 관리 방식은 한 블록의 처리를 모두 끝낸 후 최종 결과를 한꺼번에 반영하는 방식.
  //  쿼리 하나를 처리할때마다 state tree를 갱신하는 방식으로 할 수도 있음. 어느쪽이 더 적절한지 검토.
  m_us_tree.updateUserState(unresolved_block.user_ledger_list);
  m_cs_tree.updateContractState(unresolved_block.contract_ledger_list);
}

void Chain::revertStateTree(const UnresolvedBlock &unresolved_block) {
  for (auto &each_user_ledger : unresolved_block.user_ledger_list) {
    m_us_tree.insertNode(
        findUserLedgerFromPoint(each_user_ledger.first, unresolved_block.block.getHeight(), unresolved_block.cur_vec_idx).user_ledger);
  }

  for (auto &each_contract_ledger : unresolved_block.contract_ledger_list) {
    m_us_tree.insertNode(
        findContractLedgerFromPoint(each_contract_ledger.first, unresolved_block.block.getHeight(), unresolved_block.cur_vec_idx)
            .contract_ledger);
  }
}

user_ledger_type Chain::findUserLedgerFromHead(UnresolvedBlock &UR_block, const string &pid) {
  user_ledger_type return_ledger;
  return_ledger = UR_block.user_ledger_list[pid];

  if (return_ledger.is_empty) {
    shared_ptr<StateNode> node = m_us_tree.getMerkleNode(pid);
    if (node == nullptr)
      return return_ledger;
    else
      return node->getUserLedger();
  }
}

contract_ledger_type Chain::findContractLedgerFromHead(UnresolvedBlock &UR_block, const string &pid) {
  contract_ledger_type return_ledger;
  return_ledger = UR_block.contract_ledger_list[pid];

  if (return_ledger.is_empty) {
    shared_ptr<StateNode> node = m_cs_tree.getMerkleNode(pid);
    if (node == nullptr)
      return return_ledger;
    else
      return node->getContractLedger();
  }
}

void Chain::moveHead(const base58_type &target_block_id, const block_height_type target_block_height) {
  if (!target_block_id.empty()) {
    // latest_confirmed의 height가 10이었고, 현재 head의 height가 11이라면 m_block_pool[0]에 있어야 한다
    int current_deq_idx = m_head_info.deq_idx;
    int current_vec_idx = m_head_info.vec_idx;
    int current_height = static_cast<int>(m_head_info.block_height);

    while (current_height > target_block_height) {
      current_vec_idx = unresolved_block_pool->getUnresolvedBlock(current_deq_idx, current_vec_idx).prev_vec_idx;
      --current_deq_idx;
      --current_height;
    }

    vector<int> target_path = unresolved_block_pool->getPath(target_block_id, target_block_height);

    if (!target_path.empty()) {
      while (current_vec_idx != target_path[current_deq_idx]) {
        current_vec_idx = unresolved_block_pool->getUnresolvedBlock(current_deq_idx, current_vec_idx).prev_vec_idx;
        --current_deq_idx;
        --current_height;

        if (current_deq_idx < 0 && current_vec_idx < 0) {
          logger::ERROR("Target block is not linked resolved block");
          return;
        }
      }
    }

    int common_deq_idx = current_deq_idx;
    int common_vec_idx = current_vec_idx;
    int common_height = current_height;
    int front_count = static_cast<int>(target_block_height) - common_height;
    int back_count = static_cast<int>(m_head_info.block_height) - common_height;

    if (common_deq_idx < 0) {
      logger::INFO("URBP; Common block is resolved block");
    }

    current_height = static_cast<int>(m_head_info.block_height);
    current_deq_idx = m_head_info.deq_idx;
    current_vec_idx = m_head_info.vec_idx;

    for (int i = 0; i < back_count; ++i) {
      revertStateTree(unresolved_block_pool->getUnresolvedBlock(current_deq_idx, current_vec_idx));
      // TODO: ledger 이외의 것들도 revert
      current_vec_idx = unresolved_block_pool->getUnresolvedBlock(current_deq_idx, current_vec_idx).prev_vec_idx;
      --current_deq_idx;
      --current_height;
    }

    for (int i = 0; i < front_count; ++i) {
      updateStateTree(unresolved_block_pool->getUnresolvedBlock(current_deq_idx, current_vec_idx));
      // TODO: ledger 이외의 것들도 update
      ++current_deq_idx;
      current_vec_idx = target_path[current_deq_idx];
      ++current_height;
    }

    m_head_info.deq_idx = current_deq_idx;
    m_head_info.vec_idx = current_vec_idx;
    m_head_info.block_height = current_height;
    m_head_info.block_id = unresolved_block_pool->getUnresolvedBlock(m_head_info.deq_idx, m_head_info.vec_idx).block.getBlockId();

    if (unresolved_block_pool->getUnresolvedBlock(m_head_info.deq_idx, m_head_info.vec_idx).block.getHeight() != m_head_info.block_height) {
      logger::ERROR("URBP, Something error in move_head() - end part, check height");
      return;
    }

    // TODO: missing link 등에 대한 예외처리를 추가해야 할 필요 있음
  }
}

block_pool_info_type Chain::getHeadInfo() {
  return m_head_info;
}

block_pool_info_type Chain::getLongestChainInfo() {
  return m_longest_chain_info;
}

bytes Chain::getUserStateRoot() {
  return m_us_tree.getRootValue();
}

bytes Chain::getContractStateRoot() {
  return m_cs_tree.getRootValue();
}

} // namespace tethys
