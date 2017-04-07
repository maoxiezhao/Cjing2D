#ifndef _DEBUG_H_
#define _DEBUG_H_

#include"common\common.h"

/**
*	\brief ¥ÌŒÛ¥¶¿Ì
*/
namespace Debug
{
	void SetDieOnError(bool t);
	void SetPopBoxOnDie(bool t);
	void SetAbortOnDie(bool t);

	void Warning(const string& warningMsg);
	void Error(const string& errorMsg);
	void CheckAssertion(bool assertion, const string& errorMsg);
	void Die(const string& dieMsg);
}

#endif