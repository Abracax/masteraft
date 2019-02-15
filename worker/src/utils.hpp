#ifndef MASTER_RAFT_UTILS_HPP
#define MASTER_RAFT_UTILS_HPP

#include <string>
#include <functional>
#include "common.hpp"

namespace mr
{

std::string getPersistentFileName(const std::string &workerName) {
  return workerName + ".persistent";
}

std::string getConfigFileName(const std::string &workerName) {
  return workerName + ".config";
}

std::string getRoleTypeString(const Role &role) {
  switch (role)
  {
  case Role::Candidate:
    return "Candidate";
  case Role::Leader:
    return "Leader";
  case Role::Follower:
    return "Follower";
  default:
    return "Unknown";
  };
}



class Defer
{
public:
  Defer(const std::function<void()> &func)
    : _func(func)
  {}

  ~Defer() {
    _func();
  }

private:
  std::function<void()> _func;
};

}

#define MK_CLS_NAME __defer_##__FILE__##__LINE__
#define MR_DEFER(fn) Defer MK_CLS_NAME(fn)
  
#endif
