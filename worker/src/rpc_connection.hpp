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
    auto buf = new char[4];
    auto readBuf = new char[READ_BUFFER_SIZE];
    boost::asio::async_read(socket(),
              boost::asio::buffer(buf,READ_BUFFER_SIZE),boost::asio::transfer_exactly(4),
              [buf,this,readBuf](const boost::system::error_code &err,std::size_t read_length) {
                if (err) {
                      MR_LOG_WARN << "rpc read first 4 bytes error: " <<
                      err.message() << MR_EOL;
                      return;
                }
                this->socket().close();
                auto num = new char[4];
                std::memcpy(num, buf, 4);
                uint32_t len = *num;
                boost::asio::async_read(socket(),
                boost::asio::buffer(readBuf,READ_BUFFER_SIZE),boost::asio::transfer_exactly(len),
                    [readBuf,this,len](const boost::system::error_code &err,std::size_t read_length){
                      if (err) {
                        MR_LOG_WARN << "rpc read full message error: " <<
                        err.message() << MR_EOL;
                        return;
                      }
                      this->socket().close();
                      auto str = new char[len];
                      std::memcpy(str, readBuf, len);

                      auto reqBody = std::make_unique<PeerRequest>();
                      reqBody->ParseFromString(str);
                      auto term = reqBody->voterequest().term();

                      //MR_LOG << " received" << MR_EOL;

                      PeerResponse resBody;
                      VoteResponse* _resbody = new VoteResponse();
                      _resbody->set_term(term);
                      _resbody->set_votegranted(isFirst);
                      resBody.set_allocated_voteresponse(_resbody);
                      resBody.set_type(RequestVote);
                      uint32_t wlen = resBody.ByteSizeLong();
                      auto wbuf = new char[wlen + 4];
                      std::memcpy(wbuf, &wlen, 4);
                      resBody.SerializeToArray(wbuf + 4, wlen);

                      

                      boost::asio::async_write(socket(),
                              boost::asio::buffer(wbuf, wlen+4),
                                    [wbuf,wlen,this](const boost::system::error_code &err, std::size_t write_length) {
                            if (err) {
                                MR_LOG_WARN << "write error: " <<
                                err.message() << MR_EOL;
                                return;
                            }
                            //MR_LOG << "returned    " <<  write_length << MR_EOL;
                            delete[] wbuf;
                            this->socket().close();
                        });

                    });
    });
    delete[] buf;
  
  }
  void setIsFirst(bool first) {
    isFirst = first;
  }
private:
  bool isFirst;
};
}

#endif
