#pragma once

#include <string>

const int VA_INVALID_ID = -1;
const std::string VA_INVALID_ID_STRING = "-1";

namespace VA
{
	inline bool IsValidID(const int ID)
	{
		return ID >= 0;
	};
	inline bool IsValidID(const std::string& ID)
	{
		return !ID.empty() && ID != VA_INVALID_ID_STRING;
	};
}