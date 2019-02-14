#ifndef MASTER_RAFT_HTTP_SERVER_HPP
#define MASTER_RAFT_HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include "http_connection.hpp"
#include "raft_server.hpp"

namespace mr
{
class RaftServer;
class HttpStatusServer
{
public:
  HttpStatusServer(boost::asio::io_context &ctx,
	      boost::asio::ip::tcp::endpoint &ep)
    : _ctx(ctx)
    , _acceptor(_ctx, ep)
  {}

  HttpStatusServer(const HttpStatusServer &) = delete;
  HttpStatusServer &operator=(const HttpStatusServer &) = delete;
  
  virtual ~HttpStatusServer() {}

  void start()
  {
    MR_LOG << "HttpStatusServer started." << MR_EOL;
    auto p = std::make_shared<HttpConnection>(_ctx);
    
    _acceptor.async_accept(p->socket(),
			    [p, this](const boost::system::error_code &err) {
          if (err) {
            MR_LOG << "accept error: " << err.message() << MR_EOL;
            return;
          }
          p->setStateRole(_role);
          p->setStateTerm(_term);
          p->start();
          start();
        });
  }
  void setRaftServer(RaftServer *server,Role role,uint64_t term) {
    _raftServer = server;
    _role = role;
    _term = term;
  }
  RaftServer* getRaftServer(){
    return _raftServer;
  }

private:
  boost::asio::io_context &_ctx;
  boost::asio::ip::tcp::acceptor _acceptor;

  Role _role;
  uint64_t _term;
  RaftServer *_raftServer;
};

}

#endif
