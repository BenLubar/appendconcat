#include <fcntl.h>

#include <algorithm>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "util.h"
#include "state.h"

State::State(std::string filename, bool readonly) :
	messages(), sites_cache(), figures_cache(), sites_by_parent(), sites_by_distance(),
	site_to_vertex(), fout(NULL), gout(NULL), out(NULL) {
	{
		int fd_in = open(filename.c_str(), O_RDONLY);
		if (fd_in >= 0) {
			google::protobuf::io::FileInputStream fin(fd_in);
			fin.SetCloseOnDelete(true);
			google::protobuf::io::GzipInputStream gin(&fin);
			google::protobuf::io::CodedInputStream in(&gin);

			google::protobuf::uint64 len;
			std::string buf;

			bool out_of_order = false;
			appendconcat::Time time;

			while (in.ReadVarint64(&len)) {
				assert(in.ReadString(&buf, len));

				appendconcat::Message msg;
				assert(msg.ParseFromString(buf));

				if (!out_of_order) {
					if (time_compare(msg.time(), time)) {
						out_of_order = true;
					} else {
						time = msg.time();
					}
				}

				messages.push_back(msg);
			}

			if (out_of_order) {
				std::stable_sort(messages.begin(), messages.end(), message_time_compare);
			}
			update_caches_full();
		}
	}

	if (!readonly) {
		int fd_out = open(filename.c_str(), O_WRONLY|O_APPEND|O_CREAT, 0644);
		fout = new google::protobuf::io::FileOutputStream(fd_out);
		fout->SetCloseOnDelete(true);
		gout = new google::protobuf::io::GzipOutputStream(fout);
		out = new google::protobuf::io::CodedOutputStream(gout);

		bool change = false;
		appendconcat::Message msg;
		for (auto site : sites_cache) {
			// connect disconnected regions
			if (site.second.type() == appendconcat::Site::REGION && !site.second.has_parent()) {
				auto paths = find_site_paths(site.first);

				for (auto path : paths) {
					auto site_b = sites_cache.at(path.first);
					if (site_b.type() == appendconcat::Site::REGION && !site_b.has_parent() && path.second.first == std::numeric_limits<google::protobuf::int64>::max()) {
						change = true;
						auto msg_site = msg.add_sites();
						*msg_site->mutable_id() = site.first;
						auto msg_near = msg_site->add_nearby();
						*msg_near->mutable_site() = path.first;
						auto msg_time = msg_near->mutable_distance();
						msg_time->set_year(random_number(20));
						if (msg_time->year() == 0) {
							msg_time->set_month(random_number(10) + 2);
						} else {
							msg_time->set_month(random_number(12));
						}
					}
				}
			}
		}

		if (change) {
			*msg.mutable_time() = current_time;
			add(msg);
		}
	}
}

State::~State() {
	if (fout != NULL) {
		delete out;
		delete gout;
		delete fout;
	}
}

void State::add(appendconcat::Message msg) {
	assert(!read_only());

	messages.push_back(msg);
	if (time_compare(msg.time(), current_time)) {
		std::stable_sort(messages.begin(), messages.end(), message_time_compare);
		update_caches_full();
	} else {
		update_caches_one(msg);
	}
	assert(msg.SerializeToString(&buf));
	out->WriteVarint64(buf.size());
	out->WriteString(buf);
}

void State::update_caches_full() {
	figures_cache.clear();
	sites_cache.clear();
	sites_by_parent.clear();
	sites_by_distance.clear();
	site_to_vertex.clear();
	vertex_to_site.clear();
	current_time.Clear();

	for (auto msg : messages) {
		update_caches_one(msg);
	}
}

inline void State::update_caches_one(appendconcat::Message msg) {
	for (auto fig : msg.figures()) {
		figures_cache[fig.id()].MergeFrom(fig);
	}
	for (auto site : msg.sites()) {
		auto & cache = sites_cache[site.id()];
		if (site.has_parent()) {
			if (cache.has_parent()) {
				sites_by_parent[cache.parent()].erase(cache.id());
			}
			sites_by_parent[site.parent()].insert(site.id());
		}
		if (!site_to_vertex.count(site.id())) {
			site_to_vertex[site.id()] = boost::add_vertex(sites_by_distance);
			vertex_to_site[site_to_vertex[site.id()]] = site.id();
		}
		for (auto near : site.nearby()) {
			boost::remove_edge(site_to_vertex[site.id()], site_to_vertex[near.site()], sites_by_distance);
			if (near.has_distance()) {
				boost::add_edge(site_to_vertex[site.id()], site_to_vertex[near.site()], time_as_duration(near.distance()), sites_by_distance);
			}

		}
		cache.MergeFrom(site);
	}
	current_time.CopyFrom(msg.time());
}

std::unordered_map<appendconcat::UUID, std::pair<google::protobuf::int64, appendconcat::UUID> > State::find_site_paths(const appendconcat::UUID & id) const {
	auto n = boost::num_vertices(sites_by_distance);

	std::vector<boost::graph_traits<graph_t>::vertex_descriptor> predecessor(n);
	std::vector<google::protobuf::int64>                         distance(n);

	boost::dijkstra_shortest_paths(sites_by_distance, site_to_vertex.at(id),
			boost::predecessor_map(boost::make_iterator_property_map(predecessor.begin(), get(boost::vertex_index, sites_by_distance))).
			distance_map(boost::make_iterator_property_map(distance.begin(), get(boost::vertex_index, sites_by_distance))));

	std::unordered_map<appendconcat::UUID, std::pair<google::protobuf::int64, appendconcat::UUID> > result;

	boost::graph_traits<graph_t>::vertex_iterator vi, vend;
	for (tie(vi, vend) = boost::vertices(sites_by_distance); vi != vend; ++vi) {
		result[vertex_to_site.at(*vi)] = std::pair<google::protobuf::int64, appendconcat::UUID>(distance[*vi], vertex_to_site.at(predecessor[*vi]));
	}

	return result;
}
