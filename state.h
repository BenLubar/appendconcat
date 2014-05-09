#ifndef APPENDCONCAT_INCLUDED_STATE_H
#define APPENDCONCAT_INCLUDED_STATE_H

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/coded_stream.h>

#include <boost/graph/adjacency_list.hpp>

#include "util.h"

class State {
public:
	typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, boost::property<boost::vertex_name_t, std::string>, boost::property<boost::edge_weight_t, google::protobuf::int64> > graph_t;

	explicit State(std::string filename, bool readonly = false);
	~State();

	void add(appendconcat::Event);
	inline const std::vector<appendconcat::Event> & raw_events() const {
		return events;
	}
	inline const std::unordered_map<appendconcat::UUID, appendconcat::Site> & sites() const {
		return sites_cache;
	}
	inline const std::unordered_map<appendconcat::UUID, appendconcat::Figure> & figures() const {
		return figures_cache;
	}
	inline const appendconcat::Time & now() const {
		return current_time;
	}
	inline bool read_only() const {
		return out == NULL;
	}
	std::unordered_map<appendconcat::UUID, std::pair<google::protobuf::int64, appendconcat::UUID> > find_site_paths(const appendconcat::UUID &) const;
	inline const graph_t & site_graph() const {
		return sites_by_distance;
	}

private:
	std::vector<appendconcat::Event> events;
	std::unordered_map<appendconcat::UUID, appendconcat::Site> sites_cache;
	std::unordered_map<appendconcat::UUID, appendconcat::Figure> figures_cache;
	std::unordered_map<appendconcat::UUID, std::unordered_set<appendconcat::UUID> > sites_by_parent;
	std::unordered_map<appendconcat::Site::Type, std::unordered_set<appendconcat::UUID> > sites_by_type;

	graph_t sites_by_distance;
	std::unordered_map<appendconcat::UUID, boost::graph_traits<graph_t>::vertex_descriptor> site_to_vertex;
	std::unordered_map<boost::graph_traits<graph_t>::vertex_descriptor, appendconcat::UUID> vertex_to_site;

	appendconcat::Time current_time;

	void update_caches_full();
	inline void update_caches_one(const appendconcat::Event &);

	google::protobuf::io::FileOutputStream *fout;
	google::protobuf::io::GzipOutputStream *gout;
	google::protobuf::io::CodedOutputStream *out;
	std::string buf;

public:
	// Advance the world to a given time. Advancing to a time less than or equal to now() is a no-op.
	void advance(const appendconcat::Time &);

private:
	const appendconcat::UUID & random_site_by_type(appendconcat::Site::Type, const appendconcat::Time &);
};

#endif
