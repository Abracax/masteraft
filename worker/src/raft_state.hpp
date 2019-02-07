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
    : _role({Role.Candidate})
    , _term({0})
  {}

public:
  Role getRole() const {
    return _role;
  }

  void setRole(Role) {
    _role = role;
  }

  uint64_t getTerm() const {
    return _term;
  }

  void setTerm(uint64_t term) {
    _term = term;
  }

private:
  std::atomic<Role> _role;
  std::atomic_uint64_t _term;
};

}

#endif
