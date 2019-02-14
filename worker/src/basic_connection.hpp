#ifndef MASTER_RAFT_BASIC_CONNECTION_HPP
#define MASTER_RAFT_BASIC_CONNECTION_HPP

#include <boost/asio.hpp>
namespace mr
{

class BasicConnection
{
public:
  BasicConnection(boost::asio::io_context &ctx)
    : _socket(ctx)
  {}

  BasicConnection(const BasicConnection &) = delete;
  BasicConnection &operator=(const BasicConnection &) = delete;
  
  virtual ~BasicConnection() {}

public:
  virtual void start() = 0;
  
  boost::asio::ip::tcp::socket &socket() {
    return _socket;
  }

protected:
  boost::asio::ip::tcp::socket _socket;
  std::string _request;
};

}

#endif
