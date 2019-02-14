#ifndef MASTER_RAFT_RAFT_SERVER_HPP
#define MASTER_RAFT_RAFT_SERVER_HPP

#include <iostream>
#include <memory>
#include <random>
#include <boost/asio.hpp>
#include "common.hpp"
#include "http_status_server.hpp"
#include "rpc_server.hpp"
#include "raft_state.hpp"
#include "utils.hpp"
#include "src/proto/rpc.pb.h"

namespace mr
{

/**
 * top object.
 */ 
class RaftServer
{
public:
  RaftServer(const std::string &workerName, uint16_t rpcPort, uint16_t statusPort)
    : _workerName(workerName)
  {
    boost::asio::ip::tcp::endpoint httpEp(boost::asio::ip::tcp::v4(), statusPort);
    _httpStatusServer = std::make_unique<HttpStatusServer>(_ctx, httpEp);
    _httpStatusServer->setRaftServer(this,_state->getRole(),_state->getTerm());
    
    
    
    boost::asio::ip::tcp::endpoint rpcEp(boost::asio::ip::tcp::v4(), rpcPort);
    _rpcServer = std::make_unique<RpcServer>(_ctx, rpcEp);
    _rpcServer->setRaftServer(this);
    
    std::random_device rd;
    _gen.reset(new std::mt19937{rd()});
    _dist.reset(new std::uniform_int_distribution<>{
	MR_ELECT_LOW_BOUND, MR_ELECT_HIGH_BOUND});
    
    MR_LOG << "RaftServer constructed." << MR_EOL;
    MR_LOG << "current worker name: " << _workerName << MR_EOL;
  }

  RaftServer(const RaftServer &) = delete;
  RaftServer &operator=(const RaftServer &) = delete;

  void start()
  {
    MR_LOG << "start RaftServer invoked." << MR_EOL;
    _httpStatusServer->start();
    _rpcServer->start();
    setTimer();
    _ctx.run();
  }
  
  virtual ~RaftServer()
  {}

protected:
  void setTimer() {
    uint32_t time = (*_dist)(*_gen);
    MR_LOG_TRACE << "timer time: " << time << MR_EOL;
    _timer.reset(new boost::asio::steady_timer{_ctx,
	  boost::asio::chrono::milliseconds(time)});
    _timer->async_wait([this](const boost::system::error_code &err) {
	if (err) {
	  MR_LOG << "timer handler error: " << err.message() << MR_EOL;
	}
	MR_LOG_TRACE << "current role: " <<
	  getRoleTypeString(_state->getRole()) << MR_EOL;
	MR_DEFER([this]() { setTimer(); });

	if (_state->getRole() == Role::Leader) {
	  MR_LOG_TRACE << "self is leader, do nothing." << MR_EOL;
	  return;
	}
	startElection();
      });
  }

  void startElection() {
    MR_LOG << "start election" << MR_EOL;
    _state->setTerm(_state->getTerm() + 1);
    _httpStatusServer->setRaftServer(this,_state->getRole(),_state->getTerm());
    _voteFor = 0;
    _voteRejected = 0;
    
    MR_LOG << "current term: " << _state->getTerm() << MR_EOL;
    auto workers = _state->getWorkers();
    MR_LOG << "cluster worker counts in configuration: " << workers.size() << MR_EOL;
    auto it = std::find_if(workers.begin(),
        workers.end(),
        [this](const WorkerID &worker) {
          return worker.name == _workerName;
        });
    if (it == workers.end()) {
      MR_LOG_WARN << "the worker self is not configured in cluster." << MR_EOL;
    } else {
      workers.erase(it);
    }
    size_t count = workers.size();
    
    using boost::asio::ip::tcp;
    tcp::resolver resolver(_ctx);
    for (const auto &worker : workers) {
      auto eps = resolver.resolve(worker.host, std::to_string(worker.port));
      auto sock = std::make_shared<tcp::socket>(_ctx);
      boost::asio::async_connect(*sock.get(), eps,
        [this, sock, worker](const boost::system::error_code &err, const tcp::endpoint &) {
          if (err) {
            MR_LOG_WARN << "connection error to worker: " << worker.name << MR_EOL;
            MR_LOG_WARN << "connection error message: " << err.message() << MR_EOL;
            return;
          }
          auto reqBody = std::make_unique<PeerRequest>();
          auto reqBody_ = std::make_unique<VoteRequest>();
          reqBody_->set_term(_state->getTerm());
          reqBody_->set_candidatename(_workerName);
          reqBody->set_allocated_voterequest(reqBody_.get());
          uint32_t len = reqBody->ByteSizeLong();          
          // remember to free
          auto buf = new char[len + 4];
          std::memcpy(buf, &len, 4);
          reqBody->SerializeToArray(buf + 4, len);
          boost::asio::async_write(*sock.get(),
                  boost::asio::buffer(buf, len + 4),
                  [sock, buf](const boost::system::error_code &err,
                  std::size_t write_length) {
                    if (err) {
                MR_LOG_WARN <<
                  "write error: " <<
                  err.message() << MR_EOL;
                    }
                    delete[] buf;
                  });
				    // bug on reading
          auto preReadBuf = new char[4];
          boost::asio::async_read(*sock.get(),
                boost::asio::buffer(preReadBuf, 4),
                [this, sock, preReadBuf](const boost::system::error_code &err,
                    std::size_t read_length) {
                  if (err) {
                    MR_LOG_WARN << "write error: " << err.message() << MR_EOL;
                  }
                  uint32_t bodyLen = *preReadBuf;
                  MR_LOG_TRACE << "response body length: " << bodyLen << MR_EOL;
                  sock->close();
                });
          uint32_t bodyLen = *preReadBuf;
          delete[] preReadBuf;
          auto readBuf = new char[bodyLen+4];
          boost::asio::async_read(*sock.get(),
                boost::asio::buffer(readBuf, bodyLen+4),
                [this, sock, readBuf](const boost::system::error_code &err,
                    std::size_t read_length) {
                  if (err) {
                    MR_LOG_WARN <<
                "write error: " <<
                err.message() << MR_EOL;
                  }
                  auto resBody = std::make_unique<PeerResponse>();
                  resBody->ParseFromString(readBuf + 4);
                  auto term = resBody->voteresponse().term();
                  auto vote = resBody->voteresponse().votegranted();

                  if (vote) {
                    ++_voteFor;
                    MR_LOG << "got vote for me." << MR_EOL;
                  } else {
                    ++_voteRejected;
                    MR_LOG << "got vote reject me." << MR_EOL;
                  }
                  // should update term if self is not leader?
                  if (term > _state->getTerm()) {
                    _state->setTerm(term);
                    _httpStatusServer->setRaftServer(this,_state->getRole(),_state->getTerm());
                    MR_LOG << "update term from peer: " <<
                      term << MR_EOL;
                  }
                  delete[] readBuf;
                  sock->close();
                });
        });
    } // for
    while (_voteFor + _voteRejected < count) {
      sleep(10);
    }
    if (_voteFor >= (count + 1) / 2) {
      MR_LOG << "become leader." << MR_EOL;
      _state->setRole(Role::Leader);
      _httpStatusServer->setRaftServer(this,_state->getRole(),_state->getTerm());
    } else {
      MR_LOG << "vote failed." << MR_EOL;
    }
  }
  
protected:
  uint16_t _rpcPort;
  uint16_t _statusPort;
  std::string _workerName;
  boost::asio::io_context _ctx;
  std::unique_ptr<HttpStatusServer> _httpStatusServer;
  std::unique_ptr<HttpConnection> _httpConnection;
  std::unique_ptr<RpcServer> _rpcServer;
  
  std::unique_ptr<RaftState> _state{new RaftState{}};
  std::unique_ptr<std::mt19937> _gen{nullptr};
  std::unique_ptr<std::uniform_int_distribution<>> _dist{nullptr};
  std::unique_ptr<boost::asio::steady_timer> _timer{nullptr};

  std::atomic_uint32_t _voteFor;
  std::atomic_uint32_t _voteRejected;
};
  
}

#endif
