#ifndef _LOGGER_H_
#define _LOGGER_H_

#include"common\common.h"
#include<string>
#include<ostream>

/**
*	\brief ÈÕÖ¾¼ÇÂ¼
*/
namespace Logger
{
	void Print(const string& msg, std::ostream&out = std::cout);
	void Debug(const string& msg);
	void Info(const string& msg);
	void Warning(const string& msg);
	void Error(const string& msg);
	void Fatal(const string& msg);
}

#endif