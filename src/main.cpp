#include <iostream>
#include <string>
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

  if (argc != 3) {
    string usage =
      "Usage:\n"
      "worker_bin [rpc_port] [status_port]\n";
    clog << usage;
    return 1;
  }
  
  uint16_t RpcPort = std::stoi(argv[1]);
  uint16_t statusPort = std::stoi(argv[2]);
  
  return 0;
}
