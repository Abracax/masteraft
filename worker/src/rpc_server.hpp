#ifndef MASTER_RAFT_RPC_SERVER_HPP
#define MASTER_RAFT_RPC_SERVER_HPP

#include <memory>
#include "rpc_connection.hpp"

namespace mr
{

class RaftServer;
  
/**
 * Protocol:
 * First 4 bytes is a little-endian integer, indicates size of body.
 * then is a single pb bin struct act as body.
 */
class RpcServer
{
public:
  RpcServer(boost::asio::io_context &ctx,
	    boost::asio::ip::tcp::endpoint &ep)
    : _ctx(ctx)
    , _acceptor(_ctx, ep)
  {}

  RpcServer &operator=(const RpcServer &) = delete;
  RpcServer(const RpcServer &) = delete;

  virtual ~RpcServer() {}

public:
  void start() {
    MR_LOG << "RpcServer started." << MR_EOL;
    auto p = std::make_shared<RpcConnection>(_ctx);
    _acceptor.async_accept(p->socket(),
			   [p, this](const boost::system::error_code &err) {
			     if (err) {
			       MR_LOG << "accept error: " << err.message() << MR_EOL;
			       return;
			     }
			     p->start();
			     start();
			   });
  }

  void setRaftServer(RaftServer *server) {
    _raftServer = server;
  }

protected:
  boost::asio::io_context &_ctx;
  boost::asio::ip::tcp::acceptor _acceptor;

  RaftServer *_raftServer;
};

}

#endif
