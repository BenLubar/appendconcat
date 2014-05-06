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

	if (state.raw_messages().size() == 0) {
		appendconcat::Time time;

		time.set_year(-300);
		time = advance_time_random(time);

		while (time.year() < 0) {
			appendconcat::Message msg;

			*msg.mutable_time() = time;

			auto figure = msg.add_figures();

			*figure->mutable_id() = random_uuid();
			*figure->mutable_name() = random_name();
			*figure->mutable_born() = time;

			time = advance_time_random(time);

			state.add(msg);
		}

		{
			// set base time to year 0.
			appendconcat::Message msg;
			msg.mutable_time()->set_year(0);
			state.add(msg);
		}
	}

	for (auto msg : state.raw_messages()) {
		std::cout << msg.Utf8DebugString() << std::endl;
	}

	std::cout << "There are now " << state.raw_messages().size() << " messages in memory." << std::endl;

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
