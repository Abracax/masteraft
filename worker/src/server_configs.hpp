#ifndef SERVER_CONFIGS_HPP
#define SERVER_CONFIGS_HPP

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "common.hpp"

namespace mr
{

struct ConfigServer
{
    std::string serverName;
    std::string serverip;
    int serverHTTPPort;
    int serverRPCPort;
};

class ServerConfigs
{
public:
    ServerConfigs()
        :_serverData(){}

public:
    ConfigServer readJson() {
        std::ifstream t("../config/config.json");
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string str = buffer.str();
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(buffer, pt);
        boost::property_tree::ptree data = pt.get_child("Server");
        _serverData.serverName = data.get<std::string>("serverName");
        _serverData.serverip = data.get<std::string>("serverip");
        _serverData.serverHTTPPort = data.get<int>("serverHTTPPort");
        _serverData.serverRPCPort = data.get<int>("serverRPCPort");
        MR_LOG_INFO << "Configurations for this server:\n"
                    "serverName:  " << _serverData.serverName << "\n"
                    "serverip:  " << _serverData.serverip << "\n"
                    "serverRPCPort:   "<< _serverData.serverRPCPort << "\n"
                    "serverHTTPPort:  " << _serverData.serverHTTPPort << "\n" << MR_EOL;
        MR_LOG_INFO << str << MR_EOL;
        return _serverData;
    }

    virtual ~ServerConfigs(){}
private:
    ConfigServer _serverData;
};
}

#endif