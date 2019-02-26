#ifndef MASTER_RAFT_HTTP_CONNECTION_HPP
#define MASTER_RAFT_HTTP_CONNECTION_HPP

#include <string>
#include <memory>
#include <regex>
#include <sstream>
#include "basic_connection.hpp"
#include "utils.hpp"
#include "http_status_server.hpp"

namespace mr
{

class HttpStatusServer;
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
    // if request is a legal GET / request, response RaftState as json.
    // else, return 404 with {} as body.
    
    auto shared_ptr = shared_from_this();
    boost::asio::async_read_until(_socket,boost::asio::dynamic_buffer(_request),"\r\n",
                            [shared_ptr, this](const boost::system::error_code& err, size_t len){
        if(err){
            MR_LOG<<"request receiving err:"<<err.message()<<"\n" << MR_EOL;
            return;
        }
        std::string http_header = _request.substr(0, _request.find("\r\n"));
        std::regex match("(GET /) (.*)");
        if ( std::regex_match(http_header, match) ){
            char str[] = "HTTP/1.0 200 OK\r\nAccess-Control-Allow-Origin: *\r\n";
            std::ostringstream ss;
            ss<< "{\"role\":\"" << getRoleTypeString(_role) << "\"," <<"\"term\""<< ":" << _term << "}" << "\r\n\r\n";
            std::string res_str = ss.str();
            std::ostringstream ss1;
            ss1<< str << "Content-Length: " << res_str.length() << "\r\n\r\n" 
                << res_str ;
            std::string raft_state_=ss1.str();
            boost::asio::async_write(_socket, boost::asio::buffer(raft_state_), [shared_ptr, this](const boost::system::error_code& err, size_t len) {
                _socket.close();
            });
        }
        else{
            std::string err_message = "HTTP/1.0 404 Not Found\r\nContent-Type:application/json\r\n{}\r\n\r\n";
            boost::asio::async_write(_socket, boost::asio::buffer(err_message), [shared_ptr, this](const boost::system::error_code& err, size_t len) {
                _socket.close();
            });
        }
        });
    }
  
  void setStateRole(Role role) {
    _role = role;
  }

  void setStateTerm(uint64_t term){
    _term = term;
  }
private:
  Role _role;
  uint64_t _term;
  };
};

#endif
