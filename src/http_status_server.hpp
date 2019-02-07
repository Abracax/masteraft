#ifndef MASTER_RAFT_HTTP_SERVER_HPP
#define MASTER_RAFT_HTTP_SERVER_HPP

#include <boost/asio.hpp>

namespace mr
{

class HttpServer final
{
public:
  HttpServer(boost::asio::io_context &ctx,
	     boost::asio::ip::tcp::endpoint &ep)
    : _ctx(ctx)
    , _acceptor(_ctx)
  {}

  void start()
  {}

private:
  boost::asio::io_context &_ctx;
  boost::asio::ip::tcp::acceptor _acceptor;
};

}

#endif
