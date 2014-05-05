#ifndef APPENDCONCAT_INCLUDED_STATE_H
#define APPENDCONCAT_INCLUDED_STATE_H

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/coded_stream.h>

#include <vector>
#include <string>
#include <unordered_map>

#include "util.h"
#include "appendconcat.pb.h"
#include "appendconcat/uuid.pb.h"
#include "appendconcat/figure.pb.h"
#include "appendconcat/site.pb.h"
#include "appendconcat/time.pb.h"

class State {
public:
	explicit State(std::string filename, bool readonly = false);
	~State();

	void add(appendconcat::Message);
	inline const std::vector<appendconcat::Message> & raw_messages() const {
		return messages;
	}
	inline const appendconcat::Time & now() const {
		return current_time;
	}

private:
	std::vector<appendconcat::Message>                                      messages;
	std::unordered_map<appendconcat::UUID, appendconcat::Site, uuid_hash>   sites_cache;
	std::unordered_map<appendconcat::UUID, appendconcat::Figure, uuid_hash> figures_cache;
	appendconcat::Time current_time;

	void update_caches_full();
	inline void update_caches_one(appendconcat::Message);

	google::protobuf::io::FileOutputStream *fout;
	google::protobuf::io::GzipOutputStream *gout;
	google::protobuf::io::CodedOutputStream *out;
	std::string buf;
};

#endif
