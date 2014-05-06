#include <ctime>

#include <google/protobuf/io/coded_stream.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/algorithm/string.hpp> 

#include "util.h"

static boost::mt19937 random_number_(std::time(0));
static boost::uuids::random_generator random_uuid_(random_number_);

int random_number(int max) {
	return boost::random::uniform_int_distribution<>(0, max - 1)(random_number_);
}

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

std::string to_string(const appendconcat::UUID & uuid) {
	boost::uuids::uuid b;

	auto ptr = static_cast<google::protobuf::uint8 *>(b.begin());
	ptr = google::protobuf::io::CodedOutputStream::WriteLittleEndian64ToArray(uuid.high(), ptr);
	ptr = google::protobuf::io::CodedOutputStream::WriteLittleEndian64ToArray(uuid.low(), ptr);
	assert(ptr == b.end());

	return boost::uuids::to_string(b);
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

appendconcat::Name random_name_figure() {
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

static boost::random::discrete_distribution<> site_name_dist{
	30, // 0 - w{2}
	20, // 1 - w{1,2}-w{1,2}
	10, // 2 - w{1,2} of w{1,2}
	8,  // 3 - w{1,2} w{1,2}
	3,  // 4 - w{2} the w{1,2} of w{1,2}
	2,  // 5 - w{2} the w{1,2} of w-w
	2,  // 6 - w{2} the w-w of w{1,2}
	1,  // 7 - w{2} the w-w of w-w
};

static boost::random::uniform_int_distribution<> bool_dist(0, 1);

appendconcat::Name random_name_site(appendconcat::Site::Type type) {
	appendconcat::Name name;
	appendconcat::Name::Words *w = name.mutable_first();
	// TODO: for now, all types of sites are named using the same algorithm.
	switch (site_name_dist(random_number_)) {
	case 0:
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::COMPOUND);
		w = w->mutable_prefix();
		w->set_word(random_word());
		break;
	case 1:
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::HYPHEN);
		w = w->mutable_prefix();
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		break;
	case 2:
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::OF);
		w = w->mutable_prefix();
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		break;
	case 3:
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::SPACE);
		w = w->mutable_prefix();
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		break;
	case 4:
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::OF);
		w = w->mutable_prefix();
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::THE);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::COMPOUND);
		w = w->mutable_prefix();
		w->set_word(random_word());
		break;
	case 5:
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::HYPHEN);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::OF);
		w = w->mutable_prefix();
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::THE);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::COMPOUND);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::COMPOUND);
		w = w->mutable_prefix();
		w->set_word(random_word());
		break;
	case 6:
		if (bool_dist(random_number_)) {
			w->set_word(random_word());
			w->set_sep(appendconcat::Name::Words::COMPOUND);
			w = w->mutable_prefix();
		}
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::OF);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::HYPHEN);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::THE);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::COMPOUND);
		w = w->mutable_prefix();
		w->set_word(random_word());
		break;
	case 7:
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::HYPHEN);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::OF);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::HYPHEN);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::THE);
		w = w->mutable_prefix();
		w->set_word(random_word());
		w->set_sep(appendconcat::Name::Words::COMPOUND);
		w = w->mutable_prefix();
		w->set_word(random_word());
		break;
	}
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
	default:
		assert(false);
	}

	str += appendconcat::Name::Word_Name(words.word());
}

std::string to_string(const appendconcat::Name & name) {
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

inline google::protobuf::uint32 normalized_add(google::protobuf::uint32 n, int add, int *overflow, int max) {
	google::protobuf::int64 n_ = n ? n - 1 : 0;

	n_ += add;
	*overflow += n_ / max;
	n_ %= max;
	if (n_ < 0) {
		n_ += max;
		*overflow--;
	}

	return n_ + 1;
}

appendconcat::Time advance_time(appendconcat::Time time, int years, int months, int days, int seconds) {
	time.set_second(normalized_add(time.second(), seconds, &days, 24 * 60 * 60));
	time.set_day(normalized_add(time.day(), days, &months, 28));
	time.set_month(normalized_add(time.month(), months, &years, 12));
	time.set_year(time.year() + years);
	return time;
}

appendconcat::Time advance_time(appendconcat::Time time, int years, int months, int days) {
	time = advance_time(time, years, months, days, 0);
	time.clear_second();
	return time;
}

appendconcat::Time advance_time(appendconcat::Time time, int years, int months) {
	time = advance_time(time, years, months, 0, 0);
	time.clear_second();
	time.clear_day();
	return time;
}

appendconcat::Time advance_time(appendconcat::Time time, int years) {
	time = advance_time(time, years, 0, 0, 0);
	time.clear_second();
	time.clear_day();
	time.clear_month();
	return time;
}

static boost::random::uniform_int_distribution<> random_time_dist(1, 28 * 12 * 10);

appendconcat::Time advance_time_random(appendconcat::Time time) {
	return advance_time(time, 0, 0, random_time_dist(random_number_));
}
