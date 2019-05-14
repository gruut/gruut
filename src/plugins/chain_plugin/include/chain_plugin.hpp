#pragma once

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "../../../../lib/log/include/log.hpp"
#include "../../channel_interface/include/channel_interface.hpp"
#include "plugin.hpp"
#include "db_controller.hpp"

using namespace appbase;

namespace gruut {
class ChainPlugin : public Plugin<ChainPlugin> {
public:
  PLUGIN_REQUIRES()

  ChainPlugin();

  ~ChainPlugin() override;

  void pluginInitialize(const boost::program_options::variables_map &options);

  void pluginStart();

  void pluginShutdown() {
    logger::INFO("ChainPlugin Shutdown");
  }

  void setProgramOptions(options_description &cfg) override;

  void asyncFetchTransactionsFromPool();

private:
  std::unique_ptr<class ChainPluginImpl> impl;
};
} // namespace gruut
