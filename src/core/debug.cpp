#include<Windows.h>

#include"debug.h"
#include"logger.h"

namespace Debug{
	namespace
	{
		bool debugStringEnabled = false;
		bool ShowMsgBox = false; 
		bool AbortOnDie = false;
		bool DieOnError = true;
		bool debugWarningPause = true;
	}

	void SetDebugStringEnable(bool t)
	{
		debugStringEnabled = t;
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

	void DebugString(const string & warningMsg)
	{
		if (debugStringEnabled)
		{
			Logger::Debug(warningMsg);
		}
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

	void CheckAssertion(bool asertion)
	{
		if (!asertion)
			std::abort();
	}

	void Debug::CheckAssertion(bool assertion, const string & errorMsg)
	{
		if (!assertion)
			Die(errorMsg);
	}

	void Debug::Die(const string & dieMsg)
	{
		Logger::Fatal(dieMsg);
		//if(ShowMsgBox)
			//MessageBox(NULL, TEXT(dieMsg.c_str()), NULL, MB_OK);
		if (AbortOnDie)
			std::abort();
	}

	void SetDebugConsole(SHORT w, SHORT h, int bShow)
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleScreenBufferSize(handle, { w+1, (SHORT)65536 -1 });
		SMALL_RECT rc = { 0, 0, w, h - 1}; // 重置窗口位置和大小
		SetConsoleWindowInfo(handle, true, &rc);
	}

}