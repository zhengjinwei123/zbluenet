#include "base_struct.h"


#include <vector>

namespace zbluenet {
	namespace exchange {

		BaseStruct::BaseStruct()
		{

		}

		BaseStruct::~BaseStruct()
		{

		}

		std::string BaseStruct::dumpBytes(const std::string &val)
		{
			if (val.empty()) {
				return "";
			}

			std::vector<char> ret(val.size() * 3 + 1);

			for (size_t i = 0; i < val.size(); ++i) {
				::snprintf(&ret[i * 3], 4, "%02hhX-", val[i]);
			}
			ret[ret.size() - 2] = '\0';

			return std::string(&ret[0]);
		}
	} // namespace exchange
} // namespace zbluenet