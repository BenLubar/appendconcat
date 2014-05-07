#include <iostream>
#include <algorithm>

#include <boost/program_options.hpp>

#include "util.h"
#include "state.h"

int main(int argc, const char **argv) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	boost::program_options::options_description opt_desc("Options");
	opt_desc.add_options()
		("help,h", "this help message")
		("save-name,o", boost::program_options::value<std::string>()->default_value("save.gz")->value_name("file"), "save file name")
		("read-only,r", boost::program_options::bool_switch(), "do not allow modification of the save file")
		("start-history", boost::program_options::value<google::protobuf::int64>()->default_value(-300)->value_name("year"), "the earliest year in history for new games");

	boost::program_options::variables_map opts;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt_desc), opts);

	if (opts.count("help")) {
		std::cout << opt_desc << std::endl;
		return 0;
	}

	State state(opts["save-name"].as<std::string>(), opts["read-only"].as<bool>());

	if (state.raw_messages().size() == 0) {
		appendconcat::Time time;

		time.set_year(opts["start-history"].as<google::protobuf::int64>());
		time = advance_time_random(time);

		auto region = random_uuid();
		{
			appendconcat::Message msg;

			msg.mutable_time(); // don't set anything on it, we just need to allocate it.

			auto site = msg.add_sites();
			*site->mutable_id() = region;
			site->set_type(appendconcat::Site::REGION);
			*site->mutable_name() = random_name_site(site->type());

			state.add(msg);
		}

		std::vector<appendconcat::UUID> sites;

		while (time.year() < 0) {
			appendconcat::Message msg;

			*msg.mutable_time() = time;

			auto figure = msg.add_figures();

			*figure->mutable_id() = random_uuid();
			*figure->mutable_name() = random_name_figure();
			*figure->mutable_born() = time;
			figure->set_race(appendconcat::Figure::HUMAN);

			size_t i = random_number(sites.size() + 4);
			if (i < sites.size()) {
				*figure->mutable_location() = sites[i];
			} else {
				auto site = msg.add_sites();
				*site->mutable_parent() = region;
				*site->mutable_id() = random_uuid();
				site->set_type(appendconcat::Site::TOWN);
				*site->mutable_name() = random_name_site(site->type());

				sites.push_back(site->id());
				*figure->mutable_location() = site->id();

				// it's easy to go out of a town...
				auto site_near = site->add_nearby();
				*site_near->mutable_site() = site->parent();
				auto site_distance = site_near->mutable_distance();
				site_distance->set_year(0);
				site_distance->set_month(0);
				site_distance->set_day(random_number(7) + 1);

				// ... but harder to get back from an arbitrary location
				auto region_site = msg.add_sites();
				*region_site->mutable_id() = site->parent();
				auto region_near = region_site->add_nearby();
				*region_near->mutable_site() = site->id();
				auto region_distance = region_near->mutable_distance();
				region_distance->set_year(0);
				region_distance->set_month(random_number(4) + 1);
				region_distance->set_day(random_number(28));

				for (size_t j = random_number(sites.size()); j < sites.size() - 1; j += random_number(sites.size() - j)) {
					auto site_site_from = msg.add_sites();
					*site_site_from->mutable_id() = sites[j];
					auto site_near_from = site_site_from->add_nearby();
					*site_near_from->mutable_site() = site->id();
					auto site_distance_from = site_near_from->mutable_distance();
					site_distance_from->set_year(0);
					site_distance_from->set_month(random_number(3));
					if (site_distance_from->month()) {
						site_distance_from->set_day(random_number(28));
					} else {
						site_distance_from->set_day(random_number(27) + 1);
					}
					site_distance_from->set_second(random_number(24 * 60 * 60));

					auto site_near_to = site->add_nearby();
					*site_near_to->mutable_site() = sites[j];
					auto site_distance_to = site_near_to->mutable_distance();
					site_distance_to->set_year(0);
					site_distance_to->set_month(random_number(3));
					if (site_distance_to->month()) {
						site_distance_to->set_day(random_number(28));
					} else {
						site_distance_to->set_day(random_number(27) + 1);
					}
					site_distance_to->set_second(random_number(24 * 60 * 60));
				}
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
		std::string timestamp = to_string(msg.time());

		for (auto site : msg.sites()) {
			if (site.has_type() || site.has_name() || site.has_parent()) {
				auto now = state.sites().at(site.id());

				std::cout << timestamp << ", " << to_string(now.name()) << " was";

				if (site.has_type()) {
					std::cout << " a " << to_string(site.type());
				}

				if (site.has_name()) {
					std::cout << " named " << to_string(site.name());
				}

				if (site.has_parent()) {
					auto parent = state.sites().at(site.parent());
					std::cout << " in " << to_string(parent.name());
				}

				std::cout << "." << std::endl;
			}
		}

		for (auto fig : msg.figures()) {
			auto now = state.figures().at(fig.id());

			if (fig.has_born()) {
				std::string bornstamp = to_string(fig.born());
				if (bornstamp == timestamp) {
					std::string now_name = to_string(now.name());
					std::cout << timestamp << ", " << now_name << " was born";
					if (fig.has_name()) {
						std::string fig_name = to_string(fig.name());
						if (fig_name != now_name) {
							std::cout << " with the name " << fig_name;
						}
					}
					if (fig.has_location()) {
						auto site = state.sites().at(fig.location());
						std::cout << " in " << to_string(site.name()) << ", a " << to_string(site.type());
						if (site.has_parent()) {
							auto parent = state.sites().at(site.parent());
							std::cout << " in " << to_string(parent.name());
						}
					}
					std::cout << "." << std::endl;
					continue;
				}
				std::cout << timestamp << ", " << to_string(now.name()) << " was born " << bornstamp << ".";
			}

			if (fig.has_name()) {
				std::cout << timestamp << ", " << to_string(now.name()) << " was named " << to_string(fig.name()) << "." << std::endl;
			}

			if (fig.has_location()) {
				auto site = state.sites().at(fig.location());
				std::cout << timestamp << ", " << to_string(now.name()) << " was in " << to_string(site.name()) << ", a " << to_string(site.type());
				if (site.has_parent()) {
					auto parent = state.sites().at(site.parent());
					std::cout << " in " << to_string(parent.name());
				}
				std::cout << "." << std::endl;
			}
		}
	}

	std::cout << "There are now " << state.raw_messages().size() << " messages in memory." << std::endl;

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
