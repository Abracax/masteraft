#include <iostream>
#include <string>
#include <memory>
#include "raft_server.hpp"

/**
 * entry of server worker app.
 * using stdout for logging, stderr for console output.
 */
int main(int argc, char *argv[])
{
  using namespace std;
  using namespace mr;
  using namespace boost::asio;

  if (argc != 4) {
    string usage =
      "Usage:\n"
      "worker_bin [worker_name] [rpc_port] [status_port]\n";
    clog << usage;
    return 1;
  }
  
  uint16_t rpcPort = std::stoi(argv[2]);
  uint16_t statusPort = std::stoi(argv[3]);

  auto server = make_unique<RaftServer>(argv[1], rpcPort, statusPort);
  server->start();
  
  return 0;
}
