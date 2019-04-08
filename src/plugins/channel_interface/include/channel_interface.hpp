#pragma once

#include "../../../../lib/appbase/include/channel.hpp"
#include "../../../include/json.hpp"
#include "../../net_plugin/config/include/message.hpp"
#include <vector>

using namespace gruut::net_plugin;
using namespace std;

// TODO: need to separate data structure and channel.
struct InNetMsg {
  MessageType type;
  nlohmann::json body;
  sender_id_type sender_id;
};

struct OutNetMsg {
  MessageType type;
  nlohmann::json body;
  vector<sender_id_type> receivers;
};

namespace appbase::incoming {
namespace channels {
using network = ChannelTypeTemplate<InNetMsg>;
};
} // namespace appbase::incoming

namespace appbase::outgoing {
namespace channels {
using network = ChannelTypeTemplate<OutNetMsg>;
};
} // namespace appbase::outgoing
