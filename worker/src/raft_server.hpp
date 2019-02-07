#ifndef MASTER_RAFT_RAFT_SERVER_HPP
#define MASTER_RAFT_RAFT_SERVER_HPP

#include <iostream>
#include <memory>
#include <random>
#include <boost/asio.hpp>
#include "common.hpp"
#include "http_status_server.hpp"
#include "raft_state.hpp"

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

    std::random_device rd;
    _gen.reset(new std::mt19937{rd()});
    _dist.reset(new std::uniform_int_distribution<>{
	MR_ELECT_LOW_BOUND, MR_ELECT_HIGH_BOUND});
    
    MR_LOG << "RaftServer constructed." << MR_EOL;
  }

  RaftServer(const RaftServer &) = delete;
  RaftServer &operator=(const RaftServer &) = delete;

  void start()
  {
    MR_LOG << "start RaftServer invoked." << MR_EOL;
    _httpStatusServer->start();
    setTimer();
    _ctx.run();
  }
  
  virtual ~RaftServer()
  {}

protected:
  void setTimer() {
    uint32_t time = (*_dist)(*_gen);
    MR_LOG << "timer time: " << time << MR_EOL;
    _timer.reset(new boost::asio::steady_timer{_ctx,
	  boost::asio::chrono::milliseconds(time)});
    _timer->async_wait([this](const boost::system::error_code &err) {
	if (err) {
	  MR_LOG << "timer handler error: " << err.message() << MR_EOL;
	}
	setTimer();
      });
  }
  
protected:
  uint16_t _rpcPort;
  uint16_t _statusPort;
  boost::asio::io_context _ctx;
  std::unique_ptr<HttpStatusServer> _httpStatusServer;
  std::unique_ptr<RaftState> _state{new RaftState{}};
  std::unique_ptr<std::mt19937> _gen{nullptr};
  std::unique_ptr<std::uniform_int_distribution<>> _dist{nullptr};
  std::unique_ptr<boost::asio::steady_timer> _timer{nullptr};
};
  
}

#endif
