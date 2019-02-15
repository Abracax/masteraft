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
    int serverPort;
};

class ServerConfigs
{
public:
    ServerConfigs()
        :ServerData(){}

public:
    std::vector<ConfigServer> readJson() {
        std::ifstream t("src/config/config.json");
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string str = buffer.str();
        //MR_LOG_INFO << str << MR_EOL;
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(buffer, pt);
        boost::property_tree::ptree arraypt = pt.get_child("Servers");
        for (boost::property_tree::ptree::iterator it = arraypt.begin(); it != arraypt.end(); ++it) {
            boost::property_tree::ptree data = it->second;
            ConfigServer _serverData;
            _serverData.serverName = data.get<std::string>("serverName");
            _serverData.serverip = data.get<std::string>("serverip");
            _serverData.serverPort = data.get<int>("serverPort");
            ServerData.push_back(_serverData);
            MR_LOG_INFO << "Configurations for servers:\n"
                        << _serverData.serverName << "    "
                        << _serverData.serverip << "    "
                        << _serverData.serverPort << "\n"
                        << MR_EOL;
        }
        return std::move(ServerData);
    }

    virtual ~ServerConfigs(){}
private:
    std::vector<ConfigServer> ServerData;
};
}

#endif