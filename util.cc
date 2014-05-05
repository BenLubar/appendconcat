#include <google/protobuf/io/coded_stream.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "util.h"

static boost::uuids::random_generator random_uuid_;

appendconcat::UUID random_uuid() {
	appendconcat::UUID proto;
	boost::uuids::uuid uuid = random_uuid_();

	auto ptr = static_cast<const google::protobuf::uint8 *>(uuid.begin());
	google::protobuf::uint64 u64;

	ptr = google::protobuf::io::CodedInputStream::ReadLittleEndian64FromArray(ptr, &u64);
	proto.set_high(u64);
	ptr = google::protobuf::io::CodedInputStream::ReadLittleEndian64FromArray(ptr, &u64);
	proto.set_low(u64);
	assert(ptr == uuid.end());

	return proto;
}

bool time_compare(const appendconcat::Time & lhs, const appendconcat::Time & rhs) {
	if (!lhs.has_year() && rhs.has_year()) {
		return true;
	}
	if (lhs.has_year() && !rhs.has_year()) {
		return false;
	}
	if (lhs.year() < rhs.year()) {
		return true;
	}
	if (lhs.year() > rhs.year()) {
		return false;
	}

	if (!lhs.has_month() && rhs.has_month()) {
		return true;
	}
	if (lhs.has_month() && !rhs.has_month()) {
		return false;
	}
	if (lhs.month() < rhs.month()) {
		return true;
	}
	if (lhs.month() > rhs.month()) {
		return false;
	}

	if (!lhs.has_day() && rhs.has_day()) {
		return true;
	}
	if (lhs.has_day() && !rhs.has_day()) {
		return false;
	}
	if (lhs.day() < rhs.day()) {
		return true;
	}
	if (lhs.day() > rhs.day()) {
		return false;
	}

	if (!lhs.has_second() && rhs.has_second()) {
		return true;
	}
	if (lhs.has_second() && !rhs.has_second()) {
		return false;
	}
	return lhs.second() < rhs.second();
}
