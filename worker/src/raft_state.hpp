#ifndef MASTER_RAFT_RAFT_STATE_HPP
#define MASTER_RAFT_RAFT_STATE_HPP

#include <memory>
#include <atomic>
#include <string>
#include <mutex>
#include <vector>
#include "common.hpp"


namespace mr
{

struct WorkerID
{
  std::string name;
  std::string host;
  uint16_t port;
};

class RaftState final
{
public:
  RaftState()
    : _role(Role::Candidate)
    , _term(0)
  {}

public:
  Role getRole() const {
    return _role;
  }

  void setRole(Role role) {
    _role = role;
  }

  uint64_t getTerm() const {
    return _term;
  }

  void setTerm(uint64_t term) {
    _term = term;
  }
  
  using workers_t = std::vector<WorkerID>;

  workers_t getWorkers() {
    std::lock_guard<std::mutex> lock(_lock);
    return _workers;
  }

  void setWorkers(workers_t workers) {
    _workers = workers;
  }

  std::string getLeaderName() {
    std::lock_guard<std::mutex> lock(_lock);
    return _leaderName;
  }

  void setLeaderName(const std::string &name) {
    std::lock_guard<std::mutex> lock(_lock);
    _leaderName = name;
  }

private:
  std::atomic<Role> _role;
  std::atomic_uint64_t _term;
  workers_t _workers;
  std::string _leaderName;

  std::mutex _lock;
};

}

#endif
