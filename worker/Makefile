all: worker_bin

CXXFLAGS=-lpthread -lboost_system -lprotobuf -O2 -std=c++14 -g -fsanitize=address

worker_bin: src/*.cpp src/*.hpp protos
	clang++ src/*.cpp build/src/proto/*.cc -Ibuild ${CXXFLAGS} -o worker_bin

protos: src/proto/*.proto
	protoc src/proto/*.proto --cpp_out=build

.PHONY: clean

clean:
