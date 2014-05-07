#ifndef APPENDCONCAT_INCLUDED_UTIL_H
#define APPENDCONCAT_INCLUDED_UTIL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "appendconcat.pb.h"

bool time_compare(const appendconcat::Time &, const appendconcat::Time &);
bool message_time_compare(const appendconcat::Message &, const appendconcat::Message &);

extern boost::mt19937 random_number_;

template <typename IntType>
inline IntType random_number(IntType max) {
	return boost::random::uniform_int_distribution<IntType>(0, max - 1)(random_number_);
}

appendconcat::UUID random_uuid();
appendconcat::Name::Word random_word();
appendconcat::Name random_name_figure();
appendconcat::Name random_name_site(appendconcat::Site::Type);

std::string to_string(const appendconcat::UUID &);
std::string to_string(const appendconcat::Name &);
std::string to_string(const appendconcat::Time &);
std::string to_string(appendconcat::Site::Type);

google::protobuf::int64 time_as_duration(const appendconcat::Time &);

appendconcat::Time advance_time(appendconcat::Time, int, int, int, int);
appendconcat::Time advance_time(appendconcat::Time, int, int, int);
appendconcat::Time advance_time(appendconcat::Time, int, int);
appendconcat::Time advance_time(appendconcat::Time, int);

// one day to ten years
appendconcat::Time advance_time_random(appendconcat::Time);

namespace std {
	template <>
	struct hash<appendconcat::UUID> {
		inline size_t operator()(const appendconcat::UUID & uuid) const {
			return std::hash<google::protobuf::uint64>()(uuid.high() ^ uuid.low());
		}
	};

	template <>
	struct equal_to<appendconcat::UUID> {
		inline bool operator()(const appendconcat::UUID & lhs, const appendconcat::UUID & rhs) const {
			return lhs.high() == rhs.high() && lhs.low() == rhs.low();
		}
	};
}

#endif
