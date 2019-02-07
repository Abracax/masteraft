#ifndef MASTER_RAFT_HTTP_CONNECTION_HPP
#define MASTER_RAFT_HTTP_CONNECTION_HPP

#include <string>
#include <memory>

namespace mr
{

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
  HttpConnection(boost::asio::io_context &ctx)
    : _socket(ctx)
  {}

  HttpConnection(const HttpConnection &) = delete;
  HttpConnection &operator=(const HttpConnection &) = delete;
  
  virtual ~HttpConnection() {}

public:
  void start()
  {}
  
  boost::asio::ip::tcp::socket &socket() {
    return _socket;
  }

protected:
  boost::asio::ip::tcp::socket _socket;
  std::string _request;
};

}

#endif
