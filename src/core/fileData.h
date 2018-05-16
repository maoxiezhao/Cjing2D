#ifndef _FILE_DATA_H_
#define _FILE_DATA_H_

#include"common\common.h"

/**
*	\brief 资源目录管理
*	
*	使用physfs管理指定的资源目录
*/
namespace FileData
{
	bool OpenData(const string&dataName, const string& dataPath);
	void CloseData();
	bool IsOpen();

	void SetDataWriteDir(const string& writeDir);
	bool IsFileExists(const string& name);
	char* ReadFileBytes(const string& name, int& length);
	string ReadFile(const string& name);
	bool SaveFile(const string& name,const string&buffer);
	bool DeleteFile(const string& name);
	string GetPositivePath(const string& srcPath, const string& curPath);
}

#endif