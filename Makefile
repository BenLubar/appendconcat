CXXFLAGS += -Iproto $(shell pkg-config --cflags protobuf) -std=c++11 -ggdb -Wall -Wextra

proto   = proto/appendconcat.pb.o proto/appendconcat/time.pb.o \
	  proto/appendconcat/uuid.pb.o proto/appendconcat/site.pb.o \
	  proto/appendconcat/figure.pb.o proto/appendconcat/name.pb.o
objects = main.o util.o state.o $(proto)

all: appendconcat
.PHONY: all

appendconcat: $(objects)
	$(CXX) $(objects) -o $@ $(CXXFLAGS) $(LDFLAGS) -lboost_program_options $(shell pkg-config --libs protobuf) -lz

proto/.dummy: $(wildcard definitions/*.proto) $(wildcard definitions/*/*.proto)
	rm -rf proto
	mkdir proto
	protoc --cpp_out=proto -I definitions definitions/*.proto definitions/*/*.proto
	touch proto/.dummy

$(proto:%.o=%.cc) $(proto:%.o=%.h): proto/.dummy
	@touch $@

clean:
	rm -f appendconcat $(objects)
	rm -rf proto
.PHONY: clean

proto/%.pb.cc: proto/.dummy
main.o:  util.h state.h proto/.dummy
util.o:  util.h         proto/.dummy
state.o: util.h state.h proto/.dummy
