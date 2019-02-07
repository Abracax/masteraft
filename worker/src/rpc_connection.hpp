#ifndef MASTER_RAFT_RPC_CONNECTION_HPP
#define MASTER_RAFT_RPC_CONNECTION_HPP

#include <string>
#include <memory>
#include "basic_connection.hpp"

namespace mr
{

class RpcConnection : public std::enable_shared_from_this<RpcConnection>,
		      public BasicConnection
{
public:
  RpcConnection(boost::asio::io_context &ctx)
    : BasicConnection(ctx)
  {}

  void start() override {
  }
};
  
}

#endif
