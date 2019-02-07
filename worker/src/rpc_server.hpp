#ifndef MASTER_RAFT_RPC_SERVER_HPP
#define MASTER_RAFT_RPC_SERVER_HPP

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

protected:
};

}

#endif
