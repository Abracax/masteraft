#ifndef MASTER_RAFT_RPC_SERVER_HPP
#define MASTER_RAFT_RPC_SERVER_HPP

#include "rpc_connection.hpp"

namespace mr
{

/**
 * Protocol:
 * First 4 bytes is a little-endian integer, indicates size of body.
 * then is a single pb bin struct act as body.
 */
class RpcServer
{
public:
  RpcServer()
  {}

  RpcServer &operator=(const RpcServer &) = delete;
  RpcServer(const RpcServer &) = delete;

  virtual ~RpcServer() {}

public:
  void start() {
    MR_LOG << "RpcServer started." << MR_EOL;
  }

protected:
};

}

#endif
