#ifndef MASTER_RAFT_RPC_CONNECTION_HPP
#define MASTER_RAFT_RPC_CONNECTION_HPP

#include "basic_connection.hpp"
#include "src/proto/rpc.pb.h"
#include <memory>
#include <string>

namespace mr
{

class RpcConnection : public std::enable_shared_from_this<RpcConnection>,
                      public BasicConnection
{
public:
  RpcConnection(boost::asio::io_context &ctx) : BasicConnection(ctx) {}

  void start() override
  {
    auto shared_ptr = shared_from_this();
    auto buf = new char[4];

    boost::asio::async_read(
        socket(), boost::asio::buffer(buf, 4), boost::asio::transfer_exactly(4),
        [shared_ptr, buf, this](const boost::system::error_code &err,
                                std::size_t read_length) {
          if (err)
          {
            MR_LOG_WARN << "rpc read first 4 bytes error: " << err.message()
                        << MR_EOL;
            return;
          }

          auto num = new char[4];
          std::memcpy(num, buf, 4);
          uint32_t len = *num;
          //MR_LOG << "rpc pre-read length is: " << len << MR_EOL;
          auto readBuf = new char[len];
          boost::asio::async_read(
              socket(), boost::asio::buffer(readBuf, len),
              boost::asio::transfer_exactly(len),
              [shared_ptr, readBuf, this, len](const boost::system::error_code &err,
                                               std::size_t read_length) {
                if (err)
                {
                  MR_LOG_WARN
                      << "rpc read full message error: " << err.message()
                      << MR_EOL;
                  return;
                }

                auto str = new char[len]; // FIXME: delete
                std::memcpy(str, readBuf, len);

                auto reqBody = std::make_unique<PeerRequest>();
                reqBody->ParseFromArray(str,len);
                auto term = reqBody->voterequest().term();

                //MR_LOG << " received" << MR_EOL;

                PeerResponse resBody;
                VoteResponse *_resbody = new VoteResponse();
                _resbody->set_term(term);
                _resbody->set_votegranted(isFirst);
                resBody.set_allocated_voteresponse(_resbody);
                resBody.set_type(RequestVote);
                uint32_t wlen = resBody.ByteSizeLong();
                auto wbuf = new char[wlen + 4];
                std::memcpy(wbuf, &wlen, 4);
                resBody.SerializeToArray(wbuf + 4, wlen);

                boost::asio::async_write(
                    socket(), boost::asio::buffer(wbuf, wlen + 4),
                    [shared_ptr, wbuf, wlen, this](const boost::system::error_code &err,
                                                   std::size_t write_length) {
                      if (err)
                      {
                        MR_LOG_WARN << "write error: " << err.message()
                                    << MR_EOL;
                        return;
                      }
                      //MR_LOG << "returned    " << write_length << MR_EOL;
                      delete[] wbuf;
                      this->socket().close();
                    });
              });
              delete[] buf;
        });
  }
  void setIsFirst(bool first) { isFirst = first; }

private:
  bool isFirst;
};
} // namespace mr

#endif
