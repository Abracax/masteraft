#ifndef COMMON_HPP
#define COMMON_HPP

#define MR_LOG MR_LOG_INFO
#define MR_LOG_WARN std::clog << "[WARN] "
#define MR_LOG_INFO std::clog << "[INFO] "
#define MR_LOG_TRACE std::clog << "[TRACE] "
#define MR_EOL std::endl

// election term timeout
// in MS
#define MR_ELECT_LOW_BOUND 3000
#define MR_ELECT_HIGH_BOUND 5000

#define READ_BUFFER_SIZE 4096

#include <string>

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
