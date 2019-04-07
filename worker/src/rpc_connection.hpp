#ifndef MASTER_RAFT_RPC_CONNECTION_HPP
#define MASTER_RAFT_RPC_CONNECTION_HPP

#include "basic_connection.hpp"
#include "rpc.pb.h"
#include <memory>
#include <string>

namespace mr
{

class RpcConnection : public std::enable_shared_from_this<RpcConnection>,
                      public BasicConnection
{
public:
  RpcConnection(boost::asio::io_context &ctx) : BasicConnection(ctx), _grantVote(1) {}

  void start() override
  {
    auto shared_ptr = shared_from_this();
    auto buf = new char[4];

    boost::asio::async_read(
        socket(), boost::asio::buffer(buf, 4), boost::asio::transfer_exactly(4),
        [shared_ptr, buf, this](const boost::system::error_code &err,
                                std::size_t read_length) mutable{
          if (err)
          {
            MR_LOG_WARN << "rpc read first 4 bytes error: " << err.message()
                        << MR_EOL;
            return;
          }

          uint32_t len = *buf;
          auto readBuf = new char[len];

          boost::asio::async_read(
              socket(), boost::asio::buffer(readBuf, len),
              boost::asio::transfer_exactly(len),
              [shared_ptr, readBuf, this, len, buf](const boost::system::error_code &err,
                                                    std::size_t read_length) {
                if (err)
                {
                  MR_LOG_WARN
                      << "rpc read full message error: " << err.message()
                      << MR_EOL;
                  return;
                }

                auto str = new char[len]; 
                std::memcpy(str, readBuf, len);

                auto reqBody = std::make_unique<PeerRequest>();
                reqBody->ParseFromArray(str, len);
                auto term = reqBody->voterequest().term();

                if(term > _term) _grantVote = 0;

                PeerResponse resBody;
                VoteResponse *resbody = new VoteResponse();
                resbody->set_term(term);
                resbody->set_votegranted(_grantVote);
                resBody.set_allocated_voteresponse(resbody);
                resBody.set_type(RequestVote);
                uint32_t wlen = resBody.ByteSizeLong();
                auto wbuf = new char[wlen + 4];
                std::memcpy(wbuf, &wlen, 4);
                resBody.SerializeToArray(wbuf + 4, wlen);

                boost::asio::async_write(
                    socket(), boost::asio::buffer(wbuf, wlen + 4),
                    [shared_ptr, wbuf, wlen, this, readBuf, str](const boost::system::error_code &err,
                                                                 std::size_t write_length) {
                      if (err)
                      {
                        MR_LOG_WARN << "write error: " << err.message()
                                    << MR_EOL;
                        return;
                      }
                      grantVote(0);
                      delete[] wbuf;
                      delete[] readBuf;
                      delete[] str;
                      this->socket().close();
                    });
                delete[] buf;
              });
        });
  }
  void grantVote(bool first) { _grantVote = first; }
  void setState (Role role, uint64_t term){
      _role = role;
      _term = term;
  }

private:
  bool _grantVote;
  Role _role;
  uint64_t _term;
};
} // namespace mr

#endif
