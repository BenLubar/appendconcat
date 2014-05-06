#ifndef APPENDCONCAT_INCLUDED_UTIL_H
#define APPENDCONCAT_INCLUDED_UTIL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "appendconcat.pb.h"

bool time_compare(const appendconcat::Time &, const appendconcat::Time &);
bool message_time_compare(const appendconcat::Message &, const appendconcat::Message &);

int random_number(int);
appendconcat::UUID random_uuid();
appendconcat::Name::Word random_word();
appendconcat::Name random_name_figure();
appendconcat::Name random_name_site(appendconcat::Site::Type);

std::string to_string(const appendconcat::UUID &);
std::string to_string(const appendconcat::Name &);
std::string to_string(const appendconcat::Time &);
std::string to_string(appendconcat::Site::Type);

appendconcat::Time advance_time(appendconcat::Time, int, int, int, int);
appendconcat::Time advance_time(appendconcat::Time, int, int, int);
appendconcat::Time advance_time(appendconcat::Time, int, int);
appendconcat::Time advance_time(appendconcat::Time, int);

// one day to ten years
appendconcat::Time advance_time_random(appendconcat::Time);

struct uuid_hash {
	inline size_t operator()(const appendconcat::UUID & uuid) const {
		return std::hash<google::protobuf::uint64>()(uuid.high() ^ uuid.low());
	}
};

namespace std {
	inline bool operator==(const appendconcat::UUID & lhs, const appendconcat::UUID & rhs) {
		return lhs.high() == rhs.high() && lhs.low() == rhs.low();
	}
}

#endif
