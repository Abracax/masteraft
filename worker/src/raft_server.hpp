#ifndef MASTER_RAFT_RAFT_SERVER_HPP
#define MASTER_RAFT_RAFT_SERVER_HPP

#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "common.hpp"
#include "http_status_server.hpp"

namespace mr
{

/**
 * top object.
 */ 
class RaftServer
{
public:
  RaftServer(uint16_t rpcPort, uint16_t statusPort)
  {
    boost::asio::ip::tcp::endpoint httpEp(boost::asio::ip::tcp::v4(), statusPort);
    _httpStatusServer = std::make_unique<HttpStatusServer>(_ctx, httpEp);
    MR_LOG << "constructing RaftServer." << MR_EOL;
  }

  RaftServer(const RaftServer &) = delete;
  RaftServer &operator=(const RaftServer &) = delete;

  void start()
  {
    MR_LOG << "start RaftServer invoked." << MR_EOL;
    _httpStatusServer->start();
    _ctx.run();
  }
  
  virtual ~RaftServer()
  {}

protected:
  uint16_t _rpcPort;
  uint16_t _statusPort;
  boost::asio::io_context _ctx;
  std::unique_ptr<HttpStatusServer> _httpStatusServer;
};
  
}

#endif
