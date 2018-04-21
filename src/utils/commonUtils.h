#pragma once

#include<string>
#include<vector>

namespace util
{
/**
*	\brief 常用的工具函数（非数学相关）
*/
std::vector<std::string> SplitString(const std::string& str, const std::string& pattern)
{
	std::vector<std::string> ret;
	if (str.empty() || pattern.empty())
		return ret;

	std::string splitString = str + pattern; // 方便分割
	size_t pos = splitString.find(pattern);
	size_t size = splitString.size();
	while (pos != std::string::npos)
	{
		auto x = splitString.substr(0, pos);
		ret.push_back(x);
		splitString = splitString.substr(pos + 1, size);
		pos = splitString.find(pattern);
	}
	return ret;
}

}