#ifndef TETHYS_SCE_ENGINE_HPP
#define TETHYS_SCE_ENGINE_HPP

#include "../../plugins/chain_plugin/structure/block.hpp"
#include "contract_manager.hpp"
#include "data_manager.hpp"
#include "query_composer.hpp"

namespace tethys::tsce {

class ContractEngine {
public:
  ContractEngine() = default;

  void attachReadInterface(std::function<nlohmann::json(nlohmann::json &)> read_storage_interface);

  // TODO : change argument from json to Block object

  std::optional<nlohmann::json> procBlock(Block &block);

private:
  ContractManager m_contract_manager;
  QueryComposer m_query_composer;
  std::function<nlohmann::json(nlohmann::json &)> m_storage_interface;
};

} // namespace tethys::tsce

#endif // TETHYS_SCE_ENGINE_HPP
