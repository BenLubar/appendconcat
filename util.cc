#include <ctime>

#include <google/protobuf/io/coded_stream.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/algorithm/string.hpp> 

#include "util.h"

static boost::mt19937 random_number_(std::time(0));
static boost::uuids::random_generator random_uuid_(random_number_);

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

bool message_time_compare(const appendconcat::Message & lhs, const appendconcat::Message & rhs) {
	return time_compare(lhs.time(), rhs.time());
}

static boost::random::uniform_int_distribution<> word_dist(appendconcat::Name::Word_MIN+0, appendconcat::Name::Word_MAX+0);

appendconcat::Name::Word random_word() {
	auto word = static_cast<appendconcat::Name::Word>(word_dist(random_number_));
	assert(appendconcat::Name::Word_IsValid(word));
	return word;
}

appendconcat::Name random_name() {
	appendconcat::Name name;
	appendconcat::Name::Words *a = name.mutable_first();
	appendconcat::Name::Words *c = name.mutable_last();
	appendconcat::Name::Words *b = c->mutable_prefix();
	a->set_word(random_word());
	b->set_word(random_word());
	c->set_sep(appendconcat::Name::Words::COMPOUND);
	c->set_word(random_word());
	return name;
}

inline void words_string(std::string & str, const appendconcat::Name::Words & words) {
	if (words.has_prefix()) {
		words_string(str, words.prefix());
	}
	switch (words.sep()) {
	case appendconcat::Name::Words::COMPOUND:
		if (!words.has_prefix() && str.size() > 0) {
			str += " ";
		}
		// no separator
		break;
	case appendconcat::Name::Words::SPACE:
		str += " ";
		break;
	case appendconcat::Name::Words::HYPHEN:
		str += "-";
		break;
	case appendconcat::Name::Words::COMMA:
		str += ", ";
		break;
	case appendconcat::Name::Words::THE:
		str += " the ";
		break;
	case appendconcat::Name::Words::OF:
		str += " of ";
		break;
	}

	str += appendconcat::Name::Word_Name(words.word());
}

std::string name_string(const appendconcat::Name & name) {
	std::string str;

	if (name.has_first()) {
		words_string(str, name.first());
	}

	if (name.has_nick()) {
		if (str.size() > 0) {
			str += " ";
		}
		str += "\"";
		str += name.nick();
		str += "\"";
	}

	if (name.has_last()) {
		words_string(str, name.last());
	}

	boost::algorithm::to_lower(str);

	return str;
}
