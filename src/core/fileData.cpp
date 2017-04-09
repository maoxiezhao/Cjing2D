#include"fileData.h"
#include"debug.h"
#include<physfs.h>
#include<iostream>

namespace FileData
{
	string dataPath_;

	bool OpenData(const string&dataName, const string& dataPath)
	{
		if (IsOpen())
			CloseData();
		
		if (dataName.empty())
			PHYSFS_init(nullptr);
		else
			PHYSFS_init(dataName.c_str());

		dataPath_ = dataPath;
		string dirDataPath = dataPath + "/data";

		const string&baseDir = PHYSFS_getBaseDir();
		PHYSFS_addToSearchPath(dirDataPath.c_str(),1);

		return true;
	}
	
	void CloseData()
	{
		if (!IsOpen())
			return;
		dataPath_.clear();
		PHYSFS_deinit();
	}
	
	bool IsOpen()
	{
		return !dataPath_.empty();
	}

	bool IsFileExists(const string& name)
	{
		return PHYSFS_exists(name.c_str());
	}
	

	string ReadFile(const string& name)
	{
		// 确保文件存在
		Debug::CheckAssertion(PHYSFS_exists(name.c_str()),
			string("the file:") + name + " isn't exits.");

		PHYSFS_file* file = PHYSFS_openRead(name.c_str());
		Debug::CheckAssertion(file != nullptr,
			string("the file:") + name + " loaded failed.");
		
		size_t size = static_cast<size_t>(PHYSFS_fileLength(file));
		vector<char> buffer(size);

		PHYSFS_read(file, buffer.data(), 1, (PHYSFS_uint32)size);
		PHYSFS_close(file);

		return string(buffer.data(), size);
	}
	
	void SaveFile(const string& name, const string&buffer)
	{
		PHYSFS_File* file = PHYSFS_openWrite(name.c_str());
		if (file == nullptr)
			Debug::Die(string("the file:") + name + " created failed.");

		if (PHYSFS_write(file, buffer.data(), (PHYSFS_uint32)buffer.size(), 1))
			Debug::Die(string("the file:") + name + "writed failed.");

		PHYSFS_close(file);
	}

	bool DeleteFile(const string& name)
	{
		if (PHYSFS_delete(name.c_str()))
			return false;
		return true;
	}

}