#include <google/protobuf/io/coded_stream.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "proto/appendconcat/uuid.pb.h"

static boost::uuids::random_generator random_uuid_;

appendconcat::UUID *random_uuid() {
	appendconcat::UUID *proto = new appendconcat::UUID();
	boost::uuids::uuid uuid = random_uuid_();

	const google::protobuf::uint8 *ptr = static_cast<const google::protobuf::uint8 *>(uuid.begin());
	google::protobuf::uint64 u64;

	ptr = google::protobuf::io::CodedInputStream::ReadLittleEndian64FromArray(ptr, &u64);
	proto->set_high(u64);
	ptr = google::protobuf::io::CodedInputStream::ReadLittleEndian64FromArray(ptr, &u64);
	proto->set_low(u64);
	assert(ptr == uuid.end());

	return proto;
}
