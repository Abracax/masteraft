#ifndef COMMON_HPP
#define COMMON_HPP

#define MR_LOG std::clog
#define MR_EOL std::endl

namespace mr
{

enum class Role
{
  Candidate = 0,
  Leader = 1,
  Follower = 2
};

}

#endif
