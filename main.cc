#include <iostream>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/graph/graphviz.hpp>

#include "util.h"
#include "state.h"

int main(int argc, const char **argv) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	boost::program_options::options_description opt_desc("Options");
	opt_desc.add_options()
		("help,h", "this help message")
		("save-name,o", boost::program_options::value<std::string>()->default_value("save.gz")->value_name("file"), "save file name")
		("read-only,r", boost::program_options::bool_switch(), "do not allow modification of the save file")
		("start-history", boost::program_options::value<google::protobuf::int64>()->default_value(-300)->value_name("year"), "the earliest year in history for new games")
		("graphviz", "print a graphviz chart of sites in DOT format");

	boost::program_options::variables_map opts;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt_desc), opts);

	if (opts.count("help")) {
		std::cout << opt_desc << std::endl;
		return 0;
	}

	State state(opts["save-name"].as<std::string>(), opts["read-only"].as<bool>());

	if (state.raw_messages().size() == 0) {
		appendconcat::Message msg;
		msg.mutable_time()->set_year(opts["start-history"].as<google::protobuf::int64>());
		state.add(msg);
		appendconcat::Time zero;
		zero.set_year(0);
		state.advance(zero);
	}

	if (opts.count("graphviz")) {
		boost::write_graphviz(std::cout, state.site_graph(), boost::make_label_writer(boost::get(boost::vertex_name, state.site_graph())), boost::make_label_writer(boost::get(boost::edge_weight, state.site_graph())));
		return 0;
	}


	for (auto msg : state.raw_messages()) {
		std::string timestamp = to_string(msg.time());

		for (auto site : msg.sites()) {
			if (site.has_name()) {
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
						std::cout << " in ";
						if (site.has_name()) {
							std::cout << to_string(site.name()) << ", ";
						}
						std::cout << "a " << to_string(site.type());
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

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
