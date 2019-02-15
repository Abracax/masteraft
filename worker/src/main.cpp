#include <iostream>
#include <string>
#include <memory>
#include "raft_server.hpp"
#include "server_configs.hpp"

/**
 * entry of server worker app.
 * using stdout for logging, stderr for console output.
 */
int main(int argc, char *argv[])
{
  using namespace std;
  using namespace mr;
  using namespace boost::asio;

  if (argc != 2) {
    string usage =
      "Usage:\n"
      "worker_bin [rpc_port]\n";
    clog << usage;
    return 1;
  }
  uint16_t rpcPort = std::stoi(argv[1]);
  MR_LOG << "RPC port:" << rpcPort << MR_EOL;
  auto readFile = make_unique<ServerConfigs>();
  std::vector<ConfigServer> server_configs = readFile->readJson();
  for(std::vector<ConfigServer>::iterator it =  server_configs.begin(); it != server_configs.end(); ++it) {
    auto server = make_unique<RaftServer>(it->serverName, rpcPort, it->serverPort);
    server->start();
  }
  return 0;
}
