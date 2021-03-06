#ifndef TETHYS_SCE_FEE_HANDLER_HPP
#define TETHYS_SCE_FEE_HANDLER_HPP

#include "../condition_manager.hpp"
#include "../data_manager.hpp"

namespace tethys::tsce {

class FeeHandler {
public:
  FeeHandler() = default;

  std::optional<std::pair<int, int>> parseGet(std::vector<std::pair<tinyxml2::XMLElement *, std::string>> &fee_nodes,
                                              ConditionManager &condition_manager, DataManager &data_manager);
};
} // namespace tethys::tsce

#endif
