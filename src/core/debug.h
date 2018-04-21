#ifndef _DEBUG_H_
#define _DEBUG_H_

#include"common\common.h"

/**
*	\brief ¥ÌŒÛ¥¶¿Ì
*/
namespace Debug
{
	void SetDebugStringEnable(bool t);
	void SetDieOnError(bool t);
	void SetPopBoxOnDie(bool t);
	void SetAbortOnDie(bool t);

	void DebugString(const string& warningMsg);
	void Warning(const string& warningMsg);
	void Error(const string& errorMsg);
	void CheckAssertion(bool asertion);
	void CheckAssertion(bool assertion, const string& errorMsg);
	void Die(const string& dieMsg);

	void SetDebugConsole(SHORT w, SHORT h, int bShow);
}

#endif