#ifndef APPENDCONCAT_INCLUDED_STATE_H
#define APPENDCONCAT_INCLUDED_STATE_H

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/coded_stream.h>

#include "util.h"

class State {
public:
	explicit State(std::string filename, bool readonly = false);
	~State();

	void add(appendconcat::Message);
	inline const std::vector<appendconcat::Message> & raw_messages() const {
		return messages;
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

private:
	std::vector<appendconcat::Message> messages;
	std::unordered_map<appendconcat::UUID, appendconcat::Site> sites_cache;
	std::unordered_map<appendconcat::UUID, appendconcat::Figure> figures_cache;
	std::unordered_map<appendconcat::UUID, std::unordered_set<appendconcat::UUID> > sites_by_parent;
	appendconcat::Time current_time;

	void update_caches_full();
	inline void update_caches_one(appendconcat::Message);

	google::protobuf::io::FileOutputStream *fout;
	google::protobuf::io::GzipOutputStream *gout;
	google::protobuf::io::CodedOutputStream *out;
	std::string buf;
};

#endif
