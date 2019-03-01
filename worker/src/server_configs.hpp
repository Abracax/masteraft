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
#include "raft_state.hpp"

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
    ServerConfigs(const std::string &configFileName)
        : _serverData()
        , _configFileName(configFileName)
        {}

public:
    ConfigServer readJson() {
        std::ifstream t(_configFileName.c_str());
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
        boost::property_tree::ptree allWorkers = pt.get_child("AllServers");
        for (boost::property_tree::ptree::iterator it = allWorkers.begin(); it != allWorkers.end(); ++ it) {
            WorkerID worker;
            worker.name = it->second.get<std::string>("serverName");
            worker.host = it->second.get<std::string>("serverip");
            worker.port = it->second.get<int>("serverRPCPort");
            _workers.push_back(worker);
            MR_LOG_INFO << "\n"
                    << worker.name << "    "
                    << worker.host << "    "
                    << worker.port << "    "
                    << "\n" << MR_EOL;
        }
        return _serverData;
    }

    const std::vector<WorkerID>& GetAllWorkerInfo () const
    {
        return _workers;
    }

    virtual ~ServerConfigs(){}
private:
    std::string _configFileName;
    ConfigServer _serverData;
    std::vector<WorkerID> _workers;
};
}

#endif