#ifndef MASTER_RAFT_RPC_SERVER_HPP
#define MASTER_RAFT_RPC_SERVER_HPP

#include "rpc_connection.hpp"
#include "rpc.pb.h"
#include <memory>

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
  RpcServer(boost::asio::io_context &ctx, boost::asio::ip::tcp::endpoint &ep)
      : _ctx(ctx), _acceptor(_ctx, ep)
  {
  }

  RpcServer &operator=(const RpcServer &) = delete;
  RpcServer(const RpcServer &) = delete;

  virtual ~RpcServer() {}

public:
  void start()
  {
    auto RpcConnect_ptr = std::make_shared<RpcConnection>(_ctx);
    _acceptor.async_accept(
        RpcConnect_ptr->socket(),
        [RpcConnect_ptr, this](const boost::system::error_code &err) {
          if (err)
          {
            MR_LOG << "accept error: " << err.message() << MR_EOL;
            return;
          }
          RpcConnect_ptr->start();
          RpcConnect_ptr->setState(_role,_term);
          start();
        });
  }

  void setRaftServer(RaftServer *server, bool first, Role role, uint64_t term)
  {
    _raftServer = server;
    auto RpcConnect_ptr = std::make_shared<RpcConnection>(_ctx);
    RpcConnect_ptr->grantVote(first);
    _role = role;
    _term = term;
  }
protected:
  boost::asio::io_context &_ctx;
  boost::asio::ip::tcp::acceptor _acceptor;
  RaftServer *_raftServer;
  Role _role;
  uint64_t _term;
};

} // namespace mr

#endif
