#ifndef MASTER_RAFT_HTTP_CONNECTION_HPP
#define MASTER_RAFT_HTTP_CONNECTION_HPP

#include <string>
#include <memory>
#include "basic_connection.hpp"

namespace mr
{

class HttpConnection : public std::enable_shared_from_this<HttpConnection>,
		       public BasicConnection
{
public:
  HttpConnection(boost::asio::io_context &ctx)
    : BasicConnection(ctx)
  {}

public:
  void start() override
  {
    // TODO: implement
    // if request is a legal GET / request, response RaftState as json.
    // else, return 404 with {} as body.
  }
};

}

#endif
