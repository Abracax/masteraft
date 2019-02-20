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
    auto buf = new char[READ_BUFFER_SIZE];
    boost::asio::async_read(socket(),
              boost::asio::buffer(buf,READ_BUFFER_SIZE),boost::asio::transfer_at_least(4),
              [buf,this](const boost::system::error_code &err,std::size_t read_length) {
                if (err) {
                      MR_LOG_WARN << "rpc read first 4 bytes error: " <<
                      err.message() << MR_EOL;
                      return;
                }
                this->socket().close();
    });
    auto num = new char[4];
    std::memcpy(num, buf, 4);
    uint32_t len = *num;
    //MR_LOG << len << MR_EOL;

    auto str = new char[len];

    std::memcpy(str, buf+4, len);
    
    auto reqBody = std::make_unique<PeerRequest>();
    reqBody->ParseFromString(str);
    auto term = reqBody->voterequest().term();

    //MR_LOG << " received" << MR_EOL;

    PeerResponse resBody;
    VoteResponse* _resbody = new VoteResponse();
    _resbody->set_term(10);
    resBody.set_allocated_voteresponse(_resbody);
    uint32_t wlen = resBody.ByteSizeLong();
    auto wbuf = new char[wlen + 4];
    std::memcpy(wbuf, &wlen, 4);
    resBody.SerializeToArray(wbuf + 4, wlen);

    delete[] buf;

    boost::asio::async_write(socket(),
            boost::asio::buffer(wbuf, wlen+4),
                  [wbuf,wlen,this](const boost::system::error_code &err, std::size_t write_length) {
          if (err) {
              MR_LOG_WARN << "write error: " <<
              err.message() << MR_EOL;
              return;
          }
          MR_LOG << "returned    " <<  write_length << MR_EOL;
          delete[] wbuf;
          this->socket().close();
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
