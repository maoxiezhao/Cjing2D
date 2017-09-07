#include "configData.h"
#include "core\fileData.h"
#include "core\debug.h"

namespace ConfigData{

namespace{
	bool isLoadConfig = false;
}

void LoadConfig(const string & config)
{
	if (isLoadConfig)
		return;
	Debug::CheckAssertion(FileData::IsOpen(), "The FileData module is not open.");
	
	ConfigProperties& configProperties = GetConfigProperties();
	const std::string configFileName = config;

	bool result = configProperties.ImportFromFile(configFileName);
	if (result == false)
	{
		Debug::Die("Faild to open the config.");
		return;
	}

	isLoadConfig = true;
}

bool ConfigProperties::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "Config");

	lua_register(l, "config", LuaLoadConfig);
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Failed to load config datafile.");
		lua_pop(l, 1);	// ?????
		return false;
	}
	return true;
}

int ConfigProperties::LuaLoadConfig(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "Config");
		ConfigProperties* configProperties = static_cast<ConfigProperties*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		LuaTools::CheckType(l, 1, LUA_TTABLE);
		const string& title = LuaTools::CheckFieldString(l, 1, "title");
		const string& writeDir = LuaTools::CheckFieldString(l, 1, "write_dir");
		int normalWidth = LuaTools::CheckFieldInt(l, 1, "normal_width");
		int normalHeight = LuaTools::CheckFieldInt(l, 1, "normal_height");

		configProperties->SetNormalSize({ normalWidth, normalHeight });
		configProperties->SetTitle(title);
		configProperties->SetWriteDir(writeDir);

		return 0;
	});
}

ConfigProperties & ConfigProperties::GetInstance()
{
	static ConfigProperties configProperties;
	return configProperties;
}

void ConfigProperties::SetNormalSize(const Size & size) 
{
	mNormalSize = size;
}

void ConfigProperties::SetTitle(const string & title) 
{
	mTitle = title;
}

void ConfigProperties::SetWriteDir(const string & dir) 
{
	mWriteDir = dir;
}

ConfigProperties & GetConfigProperties()
{
	auto& configProperties = ConfigProperties::GetInstance();
	return configProperties;
}

Size ConfigProperties::GetNormalSize() const
{
	return mNormalSize;
}

const string ConfigProperties::GetTitle() const
{
	return mTitle;
}

const string ConfigProperties::GetWriteDir() const
{
	return mWriteDir;
}

}