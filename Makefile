CXXFLAGS += -Iproto $(shell pkg-config --cflags protobuf)

objects = main.o uuid.o proto/appendconcat.pb.o proto/appendconcat/time.pb.o proto/appendconcat/uuid.pb.o

all: appendconcat
.PHONY: all

appendconcat: $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(objects) $(shell pkg-config --libs protobuf)

proto/.dummy:
	mkdir proto
	touch proto/.dummy

proto/%.pb.cc proto/%.pb.h: definitions/%.proto proto/.dummy
	protoc --cpp_out=proto -I definitions $<

clean:
	rm -f appendconcat $(objects)
	rm -rf proto
.PHONY: clean

proto/%.pb.o: proto/%.pb.cc proto/%.pb.h
main.o: uuid.h proto/appendconcat.pb.h proto/appendconcat/time.pb.h proto/appendconcat/uuid.pb.h
uuid.o: uuid.h proto/appendconcat/uuid.pb.h
