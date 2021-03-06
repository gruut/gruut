#pragma once

#include "../../../../lib/json/include/json.hpp"
#include "../../../../lib/log/include/log.hpp"
#include "../../../../lib/tethys-utils/src/bytes_builder.hpp"
#include "../../../../lib/tethys-utils/src/cert_verifier.hpp"
#include "../../../../lib/tethys-utils/src/ecdsa.hpp"
#include "../../../../lib/tethys-utils/src/hmac_key_maker.hpp"
#include "../../../../lib/tethys-utils/src/random_number_generator.hpp"
#include "../../../../lib/tethys-utils/src/time_util.hpp"
#include "../../../../lib/tethys-utils/src/type_converter.hpp"
#include "../config/include/message.hpp"
#include "../config/include/network_config.hpp"
#include "message_builder.hpp"

#include <algorithm>
#include <atomic>
#include <optional>
#include <random>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace tethys {

using namespace net_plugin;
using namespace std;

constexpr uint64_t JOIN_TIMEOUT_SEC = 10;
constexpr int MAX_SIGNER = 200;

enum class UserMode { USER_ONLY, SIGNER_ONLY, ALL };

struct User {
  string user_id;
  string pk;
  UserMode mode;
  vector<uint8_t> hmac_key;
};

struct JoinTempData {
  string merger_nonce;
  string user_pk;
  vector<uint8_t> shared_sk;
  uint64_t start_time;
};

using TempUserPool = unordered_map<string, JoinTempData>;

class UserPool {
public:
  UserPool() = default;
  UserPool(const UserPool &) = delete;
  UserPool(const UserPool &&) = delete;
  UserPool &operator=(const UserPool &) = delete;

  void pushUser(const string &b58_user_id, vector<uint8_t> &hmac_key, const string &pk, UserMode mode) {
    User new_user;
    new_user.user_id = TypeConverter::decodeBase<58>(b58_user_id);
    new_user.hmac_key = hmac_key;
    new_user.pk = pk;
    new_user.mode = mode;

    {
      unique_lock<shared_mutex> guard(pool_mutex);
      if (mode != UserMode::USER_ONLY)
        ++num_of_signers;
      user_pool[b58_user_id] = new_user;
    }
  }

  bool eraseUser(const string &b58_user_id) {
    {
      unique_lock<shared_mutex> guard(pool_mutex);

      if (user_pool.count(b58_user_id) > 0) {
        if (user_pool[b58_user_id].mode != UserMode::USER_ONLY)
          --num_of_signers;
        user_pool.erase(b58_user_id);
        return true;
      }
      return false;
    }
  }

  optional<vector<uint8_t>> getHmacKey(const string &b58_user_id) {
    {
      shared_lock<shared_mutex> guard(pool_mutex);

      if (user_pool.count(b58_user_id) > 0)
        return user_pool[b58_user_id].hmac_key;
      return {};
    }
  }

  vector<pair<User, bitset<256>>> getCloseSigners(bitset<256> &optimal_signer_id_bitset, int size) {
    {
      shared_lock<shared_mutex> guard(pool_mutex);

      int signers_count = std::min(size, num_of_signers);

      vector<pair<User, bitset<256>>> signers;

      for (auto &[key, value] : user_pool) {
        if (value.mode != UserMode::USER_ONLY) {
          auto id_bitset = idToBitSet(TypeConverter::decodeBase<58>(value.user_id));
          signers.push_back(make_pair(value, id_bitset));
        }
      }
      sort(signers.begin(), signers.end(), [this, &optimal_signer_id_bitset](auto &left, auto &right) -> bool {
        auto left_dist = optimal_signer_id_bitset ^ left.second;
        auto right_dist = optimal_signer_id_bitset ^ right.second;

        return left_dist.count() < right_dist.count();
      });

      signers.resize(size);

      return signers;
    }
  }

  bool full() {
    {
      shared_lock<shared_mutex> guard(pool_mutex);

      return MAX_SIGNER >= num_of_signers;
    }
  }

private:
  bitset<256> idToBitSet(string_view id) {
    string bits_str;

    for (int i = 0; i < id.size(); ++i) {
      bits_str += bitset<8>(id[i]).to_string();
    }

    return bitset<256>(bits_str);
  }

  int num_of_signers{0};
  unordered_map<string, User> user_pool;
  shared_mutex pool_mutex;
};

class UserPoolManager {
public:
  UserPoolManager() = default;
  UserPoolManager(const UserPoolManager &) = delete;
  UserPoolManager(UserPoolManager &&) = default;
  UserPoolManager &operator=(UserPoolManager &&) = default;

  OutNetMsg handleMsg(InNetMsg &msg) {
    string recv_id_b58 = msg.sender_id;

    switch (msg.type) {
    case MessageType::MSG_JOIN: {
      if (!isJoinable()) {
        logger::ERROR("[ECDH] User pool is full");
        throw ErrorMsgType::ECDH_MAX_SIGNER_POOL;
      }

      auto user_id_in_body = json::get<string>(msg.body, "user");
      if (user_id_in_body.value() != msg.sender_id) {
        logger::ERROR("[ECDH] Message header and body id are different");
        throw ErrorMsgType::ECDH_ILLEGAL_ACCESS;
      }

      temp_user_pool[msg.sender_id].start_time = TimeUtil::nowBigInt();
      auto merger_nonce = TypeConverter::encodeBase<64>((RandomNumGenerator::randomize(32)));
      temp_user_pool[msg.sender_id].merger_nonce = merger_nonce;

      auto msg_challenge = MessageBuilder::build<MessageType::MSG_CHALLENGE>(recv_id_b58, merger_nonce);
      return msg_challenge;
    }
    case MessageType::MSG_RESPONSE_1: {
      if (temp_user_pool.find(msg.sender_id) == temp_user_pool.end()) {
        logger::ERROR("[ECDH] Illegal Trial");
        throw ErrorMsgType::ECDH_ILLEGAL_ACCESS;
      }
      if (isTimeout(msg.sender_id)) {
        logger::ERROR("[ECDH] Join timeout");
        throw ErrorMsgType::ECDH_TIMEOUT;
      }
      if (!verifySignature(msg.sender_id, msg.body)) {
        logger::ERROR("[ECDH] Invalid Signature");
        throw ErrorMsgType::ECDH_INVALID_SIG;
      }

      auto user_cert = json::get<string>(msg.body["user"], "pk").value();
      auto auth_cert = app().getAuthCert();
      if (!CertVerifier::doVerify(user_cert, auth_cert)) {
        logger::ERROR("[ECDH] Invalid User Certificate");
        throw ErrorMsgType::ECDH_INVALID_PK;
      }

      temp_user_pool[msg.sender_id].user_pk = user_cert;

      HmacKeyMaker key_maker;
      auto [dhx, dhy] = key_maker.getPublicKey();

      auto user_dhx = json::get<string>(msg.body["dh"], "x").value();
      auto user_dhy = json::get<string>(msg.body["dh"], "y").value();
      auto shared_sk_bytes = key_maker.getSharedSecretKey(user_dhx, user_dhy, 32);
      if (shared_sk_bytes.empty()) {
        logger::ERROR("[ECDH] Failed to generate SSK (invalid PK)");
        throw ErrorMsgType::ECDH_INVALID_PK;
      }
      temp_user_pool[msg.sender_id].shared_sk = shared_sk_bytes;

      auto un = json::get<string>(msg.body, "un").value();
      auto mn = temp_user_pool[msg.sender_id].merger_nonce;
      auto curr_time = TimeUtil::now();

      auto sig = signMessage(curr_time, mn, un, dhx, dhy, my_enc_sk, my_pass);
      auto msg_response2 = MessageBuilder::build<MessageType::MSG_RESPONSE_2>(curr_time, recv_id_b58, dhx, dhy, my_cert, sig);

      return msg_response2;
    }
    case MessageType::MSG_SUCCESS: {
      if (isTimeout(msg.sender_id)) {
        logger::ERROR("[ECDH] Join timeout");
        throw ErrorMsgType::ECDH_TIMEOUT;
      }
      auto mode = json::get<string>(msg.body, "mode").value();
      UserMode user_mode;
      if (mode == "user")
        user_mode = UserMode::USER_ONLY;
      else if (mode == "signer")
        user_mode = UserMode::SIGNER_ONLY;
      else
        user_mode = UserMode::ALL;

      user_pool.pushUser(recv_id_b58, temp_user_pool[recv_id_b58].shared_sk, temp_user_pool[recv_id_b58].user_pk, user_mode);
      temp_user_pool.erase(recv_id_b58);

      auto msg_accept = MessageBuilder::build<MessageType::MSG_ACCEPT>(recv_id_b58);
      return msg_accept;
    }
    default: {
      logger::ERROR("[ECDH] Unknown message");
      throw ErrorMsgType::UNKNOWN;
    }
    }
  }

  vector<pair<User, bitset<256>>> getCloseSigners(bitset<256> &optimal_signer, int size) {
    return user_pool.getCloseSigners(optimal_signer, size);
  }

  void removeUser(const string &b58_suser_id) {
    user_pool.eraseUser(b58_suser_id);
  }
  void removeTempUser(const string &b58_user_id) {
    if (temp_user_pool.count(b58_user_id) > 0)
      temp_user_pool.erase(b58_user_id);
  }

  optional<vector<uint8_t>> getHmacKey(const string &b58_user_id) {
    return user_pool.getHmacKey(b58_user_id);
  }

  optional<vector<uint8_t>> getTempHmacKey(const string &b58_user_id) {
    if (temp_user_pool.count(b58_user_id) > 0) {
      return temp_user_pool[b58_user_id].shared_sk;
    }
    return {};
  }

  void setSelfKeyInfo(nlohmann::json &key_info) {
    auto enc_sk = json::get<string>(key_info, "enc_sk");
    auto cert = json::get<string>(key_info, "cert");
    auto pass = json::get<string>(key_info, "pass");

    my_enc_sk = enc_sk.value();
    my_cert = cert.value();
    my_pass = pass.value();
  }

private:
  string my_enc_sk;
  string my_cert;
  string my_pass;

  bool isJoinable() {
    return user_pool.full();
  }

  bool isTimeout(const string &b58_user_id) {
    return (TimeUtil::nowBigInt() - temp_user_pool[b58_user_id].start_time) > JOIN_TIMEOUT_SEC;
  }

  bool verifySignature(const string &user_id_b58, nlohmann::json &msg_body) {
    auto dh = json::get<nlohmann::json>(msg_body, "dh");
    auto sn = json::get<string>(msg_body, "un");
    auto timestamp = json::get<string>(msg_body, "time");
    auto user_info = json::get<nlohmann::json>(msg_body, "user");
    if (!dh.has_value() || !sn.has_value() || !timestamp.has_value() || !user_info.has_value())
      return false;

    auto dhx = json::get<string>(dh.value(), "x");
    auto dhy = json::get<string>(dh.value(), "y");
    auto user_pk = json::get<string>(user_info.value(), "pk");
    auto user_b64_sig = json::get<string>(user_info.value(), "sig");
    if (!dhx.has_value() || !dhy.has_value() || !user_pk.has_value() || !user_b64_sig.has_value())
      return false;

    auto user_sig_str = TypeConverter::decodeBase<64>(user_b64_sig.value());
    auto mn = temp_user_pool[user_id_b58].merger_nonce;

    BytesBuilder bytes_builder;
    bytes_builder.appendBase<64>(mn);
    bytes_builder.appendBase<64>(sn.value());
    bytes_builder.appendHex(dhx.value());
    bytes_builder.appendHex(dhy.value());
    bytes_builder.appendDec(timestamp.value());

    return ECDSA::doVerify(user_pk.value(), bytes_builder.getBytes(), vector<uint8_t>(user_sig_str.begin(), user_sig_str.end()));
  }
  string signMessage(const string &timestamp, const string &mn, const string &sn, const string &dhx, const string &dhy,
                     const string &sk_pem, const string &sk_pass) {
    BytesBuilder bytes_builder;
    bytes_builder.appendBase<64>(mn);
    bytes_builder.appendBase<64>(sn);
    bytes_builder.appendHex(dhx);
    bytes_builder.appendHex(dhy);
    bytes_builder.appendDec(timestamp);

    auto sig = ECDSA::doSign(sk_pem, bytes_builder.getBytes(), sk_pass);
    return TypeConverter::encodeBase<64>(sig);
  }

  UserPool user_pool;
  TempUserPool temp_user_pool;
};

} // namespace tethys