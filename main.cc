#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "appendconcat.pb.h"
#include "util.h"
#include "state.h"

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	State state("save.gz");

	for (int i = 0; i < 10; i++) {
		appendconcat::Message msg;

		*msg.mutable_time() = advance_time(state.now(), 0, 0, 1);

		auto figure = msg.add_figures();

		*figure->mutable_id() = random_uuid();
		*figure->mutable_name() = random_name();

		state.add(msg);
	}

	for (auto msg : state.raw_messages()) {
		std::cout << msg.Utf8DebugString() << std::endl;
	}

	std::cout << "There are now " << state.raw_messages().size() << " messages in memory." << std::endl;

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
