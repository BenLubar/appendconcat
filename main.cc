#include <fcntl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "proto/appendconcat.pb.h"
#include "uuid.h"

bool message_time_compare(appendconcat::Message a, appendconcat::Message b) {
	appendconcat::Time ta = a.time();
	appendconcat::Time tb = b.time();

	if (ta.year() < tb.year()) {
		return true;
	}
	if (ta.year() > tb.year()) {
		return false;
	}

	if (ta.month() < tb.month()) {
		return true;
	}
	if (ta.month() > tb.month()) {
		return false;
	}

	if (ta.day() < tb.day()) {
		return true;
	}
	if (ta.day() > tb.day()) {
		return false;
	}

	return ta.second() < tb.second();
}

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	appendconcat::UUID *uuid = random_uuid();
	std::cout << uuid->Utf8DebugString() << std::endl;
	delete uuid;

	{
		int fd_in = open("save.gz", O_RDONLY);
		google::protobuf::io::FileInputStream fin(fd_in);
		fin.SetCloseOnDelete(true);
		google::protobuf::io::GzipInputStream gin(&fin);
		google::protobuf::io::CodedInputStream in(&gin);

		google::protobuf::uint64 len;
		std::string buf;

		std::vector<appendconcat::Message> messages;

		for (;;) {
			if (!in.ReadVarint64(&len)) {
				break;
			}
			if (!in.ReadString(&buf, len)) {
				std::cerr << "internal error" << std::endl;
				return 1;
			}

			appendconcat::Message msg;

			if (!msg.ParseFromString(buf)) {
				std::cerr << "internal error" << std::endl;
				return 1;
			}

			messages.push_back(msg);
		}

		std::stable_sort(messages.begin(), messages.end(), message_time_compare);

		for (std::vector<appendconcat::Message>::iterator it = messages.begin(); it != messages.end(); it++) {
			std::cout << it->Utf8DebugString() << std::endl;
		}

		std::cout << "There are now " << messages.size() << " messages in memory." << std::endl;
	}

	{
		int fd_out = open("save.gz", O_WRONLY|O_APPEND|O_CREAT, 0644);
		google::protobuf::io::FileOutputStream fout(fd_out);
		fout.SetCloseOnDelete(true);
		google::protobuf::io::GzipOutputStream gout(&fout);
		google::protobuf::io::CodedOutputStream out(&gout);

		std::string buf;

		for (int i = 1; i <= 10; i++) {
			appendconcat::Message msg;
			appendconcat::Time *time = msg.mutable_time();
			time->set_year(i);

			if (!msg.SerializeToString(&buf)) {
				std::cerr << "internal error" << std::endl;
				return 1;
			}

			out.WriteVarint64(buf.size());
			out.WriteString(buf);
		}
	}

	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
