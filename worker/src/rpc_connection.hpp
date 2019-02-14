#ifndef MASTER_RAFT_RPC_CONNECTION_HPP
#define MASTER_RAFT_RPC_CONNECTION_HPP

#include <string>
#include <memory>
#include "basic_connection.hpp"
#include "src/proto/rpc.pb.h"

namespace mr
{

class RpcConnection : public std::enable_shared_from_this<RpcConnection>,
		      public BasicConnection
{
public:
  RpcConnection(boost::asio::io_context &ctx)
    : BasicConnection(ctx)
  {}

  void start() override {
    auto p = shared_from_this();
    uint32_t realSize;
    boost::asio::async_read(socket(),
			    boost::asio::buffer(&realSize, 4),
			    [](const boost::system::error_code &err, size_t) {
            if (err) {
              MR_LOG_WARN <<
          "rpc receive error: " <<
          err.message() << MR_EOL;
            }
          });
    auto readBuf = new char[realSize+4];
    boost::asio::ip::tcp::socket& sock = socket();
    boost::asio::async_read(socket(),
          boost::asio::buffer(readBuf, realSize+4),
          [this,readBuf,&sock](const boost::system::error_code &err,
              std::size_t read_length) {
            if (err) {
              MR_LOG_WARN <<
          "rpc read error: " <<
          err.message() << MR_EOL;
            }
            auto reqBody = std::make_unique<PeerRequest>();
            reqBody->ParseFromString(readBuf + 4);
            auto term = reqBody->voterequest().term();
            auto resBody = std::make_unique<PeerResponse>();
            auto _resbody = std::make_unique<VoteResponse>();
            _resbody->set_term(term);
            _resbody->set_votegranted(isFirst);
            resBody->set_type(RequestVote);
            resBody->set_allocated_voteresponse(_resbody.get());
            uint32_t len = resBody->ByteSizeLong();
            auto buf = new char[len + 4];
            std::memcpy(buf, &len, 4);
            resBody->SerializeToArray(buf + 4, len);
            boost::asio::async_write(socket(),
                  boost::asio::buffer(buf, len + 4),
                  [&sock, buf](const boost::system::error_code &err,
                  std::size_t write_length) {
                    if (err) {
                MR_LOG_WARN <<
                  "write error: " <<
                  err.message() << MR_EOL;
                    }
                    delete[] buf;
                  });
            sock.close();
            delete[] readBuf;
          });
  }
  void setIsFirst(bool first) {
    isFirst = first;
  }
private:
  bool isFirst;
};
}

#endif
