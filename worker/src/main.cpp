#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
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
  std::string fileName;
  if(argc != 2){
    MR_LOG_WARN << "Please input a valid file name" << MR_EOL;
    return 1;
  } 
  else{
    fileName = argv[1];
  }

  auto readFile = make_unique<ServerConfigs>(fileName);
  ConfigServer server_configs = readFile->readJson();
  std::vector<WorkerID> workerInfo = readFile->GetAllWorkerInfo();
  auto server = make_unique<RaftServer>(server_configs.serverName, server_configs.serverRPCPort, server_configs.serverHTTPPort,workerInfo);
  try {
     server->start();
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
 
  return 0;
}
