#ifndef APPENDCONCAT_INCLUDED_UTIL_H
#define APPENDCONCAT_INCLUDED_UTIL_H

#include "appendconcat.pb.h"
#include "appendconcat/uuid.pb.h"
#include "appendconcat/time.pb.h"

appendconcat::UUID random_uuid();
bool time_compare(const appendconcat::Time &, const appendconcat::Time &);
inline bool message_time_compare(const appendconcat::Message & lhs, const appendconcat::Message & rhs) {
	return time_compare(lhs.time(), rhs.time());
}

struct uuid_hash {
	inline size_t operator()(const appendconcat::UUID & uuid) const {
		return reinterpret_cast<size_t>(uuid.high() ^ uuid.low());
	}
};

namespace std {
	inline bool operator==(const appendconcat::UUID & lhs, const appendconcat::UUID & rhs) {
		return lhs.high() == rhs.high() && lhs.low() == rhs.low();
	}
}

#endif
