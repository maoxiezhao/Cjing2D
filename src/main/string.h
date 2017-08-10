#pragma once

#include"common\common.h"
#include<sstream>

namespace String
{
	string ToString(int value)
	{
		std::stringstream oss;
		oss << value;
		return oss.str();
	}
}