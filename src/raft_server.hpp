#ifndef MASTER_RAFT_RAFT_SERVER_HPP
#define MASTER_RAFT_RAFT_SERVER_HPP

#include <boost/asio.hpp>
#include "http_status_server.hpp"

namespace mr
{

/**
 * top object.
 */ 
class RaftServer
{
public:
  RaftServer()
  {}

  RaftServer(const RaftServer &) = delete;
  RaftServer &operator=(const RaftServer &) = delete;
  
  virtual ~RaftServer()
  {}

protected:
};
  
}

#endif
