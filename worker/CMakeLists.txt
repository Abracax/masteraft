cmake_minimum_required(VERSION 3.7)
project(masteraft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-lpthread -lboost_system -lboost_date_time -O2 -std=c++17 -g \
-I/usr/local/Cellar/boost/1.68.0_1/include \
-L/usr/local/Cellar/boost/1.68.0_1/lib")

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS date_time log thread system)
find_package(Protobuf REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})


file(GLOB PB_SRCS
        "src/proto/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PB_SRCS})

file(GLOB BIN_HEADERS
        "src/*.hpp")

add_executable(worker_bin
        src/main.cpp
        ${BIN_HEADERS}
        ${PROTO_SRCS}
        ${PROTO_HDRS})

target_link_libraries(worker_bin ${Protobuf_LIBRARIES})
