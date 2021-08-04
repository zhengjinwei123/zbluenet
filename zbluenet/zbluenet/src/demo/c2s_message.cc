#include "c2s_message.h"

#include <zbluenet/exchange/macro.h>
#include <sstream>
#include <string>

C2SLoginRequest::C2SLoginRequest()
{

}

C2SLoginRequest::~C2SLoginRequest()
{

}

void C2SLoginRequest::swap(C2SLoginRequest &other)
{
	this->data.Swap(&other.data);
}

int C2SLoginRequest::encode(char *buffer, size_t size) const
{
	if (!data.SerializeToArray(buffer, size)) {
		return -1;
	}

	return data.ByteSizeLong();
}

int C2SLoginRequest::decode(const char *buffer, size_t size)
{
	std::string str(buffer, size);

	if (false == data.ParseFromString(str)) {
		return 0;
	}

	return size;
}

std::string C2SLoginRequest::dump() const
{
	std::stringstream ss;

	ss << "account:\"" << this->data.account() << "\",";
	ss << "age:\"" << this->data.age() << "\",";

	if (this->data.friends_size() > 0) {
		ss << "friends:[\"";
		for (int i = 0; i < this->data.friends_size(); ++i) {
			if (i == this->data.friends_size()-1) {
				ss << this->data.friends(i);
			} else {
				ss << this->data.friends(i) << ",";
			}
		}
		ss << "]\"";
	} else {
		ss << "friends:\"" << "[]\"";
	}

	std::string s = ss.str();
	if (s.empty() == false) {
		s.erase(s.end() - 1);
	}

	return s;
}

