all: worker_bin

CXXFLAGS=-lpthread -lboost_system -O2 -g

worker_bin: src/*.cpp src/*.hpp protos
	g++ src/*.cpp ${CXXFLAGS} -o worker_bin

protos: src/proto/*.proto
	protoc src/proto/rpc.proto --cpp_out=build

.PHONY: clean

clean:
