#ifndef MASTER_RAFT__HPP
#define MASTER_RAFT__HPP

#include <string>
#include <memory>

namespace mr
{

class RpcConnection : public std::enable_shared_from_this<RpcConnection>
{
public:
  RpcConnection(boost::asio::io_context &ctx) {}

};
  
}

#endif
