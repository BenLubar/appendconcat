#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "appendconcat.pb.h"
#include "util.h"
#include "state.h"

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	appendconcat::UUID uuid = random_uuid();
	std::cout << uuid.Utf8DebugString() << std::endl;

	State state("save.gz");

	for (auto msg : state.raw_messages()) {
		std::cout << msg.Utf8DebugString() << std::endl;
	}

	std::cout << "There are now " << state.raw_messages().size() << " messages in memory." << std::endl;

	for (int i = 0; i < 10; i++) {
		appendconcat::Message msg;
		appendconcat::Time *time = msg.mutable_time();
		*time = state.now();
		time->set_year(time->year() + 1);

		state.add(msg);
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
