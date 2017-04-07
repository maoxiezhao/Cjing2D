#include<Windows.h>

#include"debug.h"
#include"logger.h"

namespace Debug{
	namespace
	{
		bool ShowMsgBox = false; 
		bool AbortOnDie = false;
		bool DieOnError = false;
	}

	void SetDieOnError(bool t)
	{
		DieOnError = t;
	}

	void SetPopBoxOnDie(bool t)
	{
		ShowMsgBox = t;
	}

	void SetAbortOnDie(bool t)
	{
		AbortOnDie = t;
	}

	void Debug::Warning(const string & warningMsg)
	{
		Logger::Warning(warningMsg);
	}

	void Debug::Error(const string & errorMsg)
	{
		Logger::Error(errorMsg);
		if (DieOnError)
			abort();
	}

	void Debug::CheckAssertion(bool assertion, const string & errorMsg)
	{
		if (!assertion)
			Die(errorMsg);
	}

	void Debug::Die(const string & dieMsg)
	{
		Logger::Fatal(dieMsg);
		if(ShowMsgBox)
			MessageBox(NULL, TEXT(dieMsg.c_str()), NULL, MB_OK);
		if (AbortOnDie)
			std::abort();
	}

}