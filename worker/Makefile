all: worker_bin

CXXFLAGS=-lpthread -lboost_system -lprotobuf -lboost_date_time -O2 -std=c++17 -g

worker_bin: src/*.cpp src/*.hpp protos
	clang++ src/*.cpp build/src/proto/*.cc -Ibuild ${CXXFLAGS} -o worker_bin

protos: src/proto/*.proto
	protoc src/proto/*.proto --cpp_out=build

clean: 
	-rm -f build/src/proto/*.pb.cc build/src/proto/*.pb.h worker_bin
	-rm -rf worker_bin.dSYM
