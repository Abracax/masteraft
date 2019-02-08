#ifndef MASTER_RAFT_RAFT_STATE_HPP
#define MASTER_RAFT_RAFT_STATE_HPP

#include <memory>
#include <atomic>
#include "common.hpp"

namespace mr
{

class RaftState final
{
public:
  RaftState()
    : _role({Role::Candidate})
    , _term({0})
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
  
  using workers_t = std::vector<std::pair<std::string, uint16_t>>;

  workers_t &getWorkers() {
    return _workers;
  }

private:
  std::atomic<Role> _role;
  std::atomic_uint64_t _term;
  workers_t _workers;

  std::mutex _lock;
};

}

#endif
