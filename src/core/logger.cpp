#include "logger.h"
#include<fstream>

namespace Logger {
	namespace {
		const string errorLogFileName = "error.txt";
		std::ofstream errorFile;
	
		std::ofstream& GetErrorFile()
		{
			if (!errorFile.is_open())
				errorFile.open(errorLogFileName);
			return errorFile;
		}
	}
	
	void Logger::Print(const string & msg, std::ostream & out)
	{
		out << msg << std::endl;
	}

	void Debug(const string & msg)
	{
		Print("Debug:" + msg);
	} 

	void Logger::Info(const string & msg)
	{
		Print("Info:" + msg);
	}

	void Logger::Warning(const string & msg)
	{
		string warningMsg = "Warning:" + msg;
		Print(warningMsg);
		Print(warningMsg, GetErrorFile());
	}

	void Logger::Error(const string & msg)
	{
		string warningMsg = "Error:" + msg;
		Print(warningMsg);
		Print(warningMsg, GetErrorFile());
	}

	void Fatal(const string& msg)
	{
		string warningMsg = "Fatal:" + msg;
		Print(warningMsg);
		Print(warningMsg, GetErrorFile());
	}
}
