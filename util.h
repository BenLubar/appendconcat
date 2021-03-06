#ifndef APPENDCONCAT_INCLUDED_UTIL_H
#define APPENDCONCAT_INCLUDED_UTIL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "event.pb.h"

bool time_compare(const appendconcat::Time &, const appendconcat::Time &);
bool event_time_compare(const appendconcat::Event &, const appendconcat::Event &);

extern boost::mt19937 random_number_;

template <typename IntType>
inline IntType random_number(IntType min, IntType max) {
	return boost::random::uniform_int_distribution<IntType>(min, max)(random_number_);
}

template <typename IntType>
inline IntType random_number(IntType max) {
	return random_number(IntType(0), max - 1);
}

appendconcat::UUID random_uuid();
appendconcat::Name::Word random_word();
appendconcat::Name random_name_figure();
appendconcat::Name random_name_site(appendconcat::Site::Type);

std::string to_string(const appendconcat::UUID &);
std::string to_string(const appendconcat::Name &);
std::string to_string(const appendconcat::Time &);
std::string to_string(appendconcat::Figure::Modifier);
std::string to_string(appendconcat::Site::Type);

google::protobuf::int64 time_as_duration(const appendconcat::Time &);

appendconcat::Time advance_time(appendconcat::Time, int, int, int, int);
appendconcat::Time advance_time(appendconcat::Time, int, int, int);
appendconcat::Time advance_time(appendconcat::Time, int, int);
appendconcat::Time advance_time(appendconcat::Time, int);

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

	template <>
	struct hash<appendconcat::Site::Type> {
		inline size_t operator()(const appendconcat::Site::Type & type) const {
			return std::hash<google::protobuf::uint64>()(type + 0);
		}
	};
}

#endif
