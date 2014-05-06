#include <fcntl.h>

#include "state.h"
#include "util.h"

State::State(std::string filename, bool readonly) :
	messages(), sites_cache(), figures_cache(), sites_by_parent(),
	fout(NULL), gout(NULL), out(NULL) {
	{
		int fd_in = open(filename.c_str(), O_RDONLY);
		if (fd_in >= 0) {
			google::protobuf::io::FileInputStream fin(fd_in);
			fin.SetCloseOnDelete(true);
			google::protobuf::io::GzipInputStream gin(&fin);
			google::protobuf::io::CodedInputStream in(&gin);

			google::protobuf::uint64 len;
			std::string buf;

			while (in.ReadVarint64(&len)) {
				assert(in.ReadString(&buf, len));

				appendconcat::Message msg;
				assert(msg.ParseFromString(buf));
				messages.push_back(msg);
			}
		}

		update_caches_full();
	}

	if (!readonly) {
		int fd_out = open(filename.c_str(), O_WRONLY|O_APPEND|O_CREAT, 0644);
		fout = new google::protobuf::io::FileOutputStream(fd_out);
		fout->SetCloseOnDelete(true);
		gout = new google::protobuf::io::GzipOutputStream(fout);
		out = new google::protobuf::io::CodedOutputStream(gout);
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
	assert(out);

	messages.push_back(msg);
	if (time_compare(msg.time(), current_time)) {
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
	current_time.Clear();

	std::stable_sort(messages.begin(), messages.end(), message_time_compare);

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
		cache.MergeFrom(site);
	}
	current_time.CopyFrom(msg.time());
}
