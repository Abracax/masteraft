#ifndef MASTER_RAFT_HTTP_CONNECTION_HPP
#define MASTER_RAFT_HTTP_CONNECTION_HPP

#include <string>
#include <memory>
#include <regex>
#include <sstream>
#include "basic_connection.hpp"
#include "utils.hpp"
#include "raft_state.hpp"


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
    auto shared_ptr = shared_from_this();
    boost::asio::async_read_until(BasicConnection::_socket,boost::asio::dynamic_buffer(BasicConnection::_request),"\r\n\r\n",
                           [shared_ptr, this](const boost::system::error_code& err, size_t len){
        if(err){
            MR_LOG<<"request receiving err:"<<err.message()<<"\n" << MR_EOL;
            return;
        }
        std::string http_header = BasicConnection::_request.substr(0, _request.find("\r\n"));
        std::regex match("(GET /) (.*)");
        if ( std::regex_match(http_header, match) ){
            char str[] = "HTTP/1.0 200 OK\r\nContent-Type:application/json\r\n";
            std::ostringstream ss;
            RaftState state;
            Role role = state.getRole();
            ss<< str << "{\"role\":\""<< getRoleTypeString(role) << "\"," <<"\"term\""<< ":" << state.getTerm() << "}" << "\r\n\r\n";
            std::string raft_state_ = ss.str();
            boost::asio::async_write(BasicConnection::_socket, boost::asio::buffer(raft_state_), [shared_ptr, this](const boost::system::error_code& err, size_t len) {
                BasicConnection::_socket.close();
            });
        }
        else{
            std::string err_message = "HTTP/1.0 404 Not Found\r\nContent-Type:application/json\r\n{}\r\n\r\n";
            boost::asio::async_write(BasicConnection::_socket, boost::asio::buffer(err_message), [shared_ptr, this](const boost::system::error_code& err, size_t len) {
                BasicConnection::_socket.close();
            });
        }
        });
    }
  };
};

#endif
