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

  auto readFile = make_unique<ServerConfigs>();
  ConfigServer server_configs = readFile->readJson();
  
  auto server = make_unique<RaftServer>(server_configs.serverName, server_configs.serverRPCPort, server_configs.serverHTTPPort);
  server->start();
  return 0;
}
