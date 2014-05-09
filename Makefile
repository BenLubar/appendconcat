CXXFLAGS += -Iproto $(shell pkg-config --cflags protobuf) -std=c++11 -ggdb -Wall -Wextra -O2 -pedantic -Dickweed

proto   = proto/event.pb.o proto/time.pb.o proto/uuid.pb.o proto/name.pb.o proto/site.pb.o \
	  proto/figure.pb.o
objects = main.o util.o state.o advance.o $(proto)

all: appendconcat
.PHONY: all

appendconcat: $(objects)
	$(CXX) $(objects) -o $@ $(CXXFLAGS) $(LDFLAGS) -lboost_program_options $(shell pkg-config --libs protobuf) -lz

proto/.dummy: $(wildcard definitions/*.proto)
	rm -rf proto
	mkdir proto
	protoc --cpp_out=proto -I definitions definitions/*.proto
	touch proto/.dummy

$(proto:%.o=%.cc) $(proto:%.o=%.h): proto/.dummy
	@touch $@

clean:
	rm -f appendconcat $(objects)
	rm -rf proto
.PHONY: clean

proto/%.pb.cc: proto/.dummy
main.o:    util.h state.h proto/.dummy
util.o:    util.h         proto/.dummy
state.o:   util.h state.h proto/.dummy
advance.o: util.h state.h proto/.dummy
