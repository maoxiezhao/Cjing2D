#include "luaData.h"
#include "core\debug.h"
#include "core\fileData.h"

bool LuaData::ImportFromFile(const string & filename)
{
	// 是否有必要使用fileData
	if (!FileData::IsFileExists(filename))
	{
		Debug::Error("The lua file " + filename + "not existed.");
		return false;
	}

	mFileName = filename;
	mFilePath = "";
	int pos = filename.rfind("/");
	if (pos != std::string::npos)
	{
		mFileName = filename.substr(pos + 1);
		mFilePath = filename.substr(0, pos + 1);
	}
	
	const string buffer = FileData::ReadFile(filename);
	lua_State* l = luaL_newstate();
	if ( luaL_loadbuffer(l, buffer.data(), buffer.size(), filename.c_str()) != 0)
	{
		Debug::Error("Can't load lua data file:" + filename);
		lua_pop(l, 1);
		return false;
	}
	bool success = ImportFromLua(l);
	lua_close(l);
	return success;
}

std::string LuaData::GetFileName() const
{
	return mFileName;
}

std::string LuaData::GetFilePath() const
{
	return mFilePath;
}
