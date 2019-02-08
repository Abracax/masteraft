#ifndef MASTER_RAFT_UTILS_HPP
#define MASTER_RAFT_UTILS_HPP

#include <string>

namespace mr
{

std::string getPersistentFileName(const std::string &workerName) {
  return workerName + ".persistent";
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
  
}

#endif
