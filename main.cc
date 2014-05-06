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

		std::vector<appendconcat::UUID> sites;

		while (time.year() < 0) {
			appendconcat::Message msg;

			*msg.mutable_time() = time;

			auto figure = msg.add_figures();

			*figure->mutable_id() = random_uuid();
			*figure->mutable_name() = random_name_figure();
			*figure->mutable_born() = time;

			int i = random_number(sites.size() + 4);
			if (i < sites.size()) {
				*figure->mutable_location() = sites[i];
			} else {
				auto site = msg.add_sites();
				*site->mutable_id() = random_uuid();
				site->set_type(appendconcat::Site::TOWN);
				*site->mutable_name() = random_name_site(site->type());

				sites.push_back(site->id());
				*figure->mutable_location() = site->id();
			}

			state.add(msg);

			time = advance_time_random(time);
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
