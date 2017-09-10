#include"savegame.h"
#include"lua\luaContext.h"
#include"core\fileData.h"
#include<sstream>

Savegame::Savegame(const string& fileName):
	mFileName(fileName),
	mGame(nullptr)
{
}

void Savegame::Init()
{
	if (!FileData::IsFileExists(mFileName))
	{
		SetDefaultData();
	}
	else
	{
		ImportFromFile(mFileName);
	}
}

void Savegame::SetGame(Game * game)
{
	mGame = game;
}

Game * Savegame::GetGame()
{
	return mGame;
}

/**
*	\brief 保存游戏存档到本地
*
*	以 xx = value 的形式保存文档数据
*/
void Savegame::SaveGameToLocal()
{
	std::ostringstream oss;
	for (const auto& kvp : mSavedValues)
	{
		const string& key = kvp.first;
		oss << key << " = ";
		const auto& savedValue = kvp.second;
		if (savedValue.type == SavedValue::VALUE_INTEGER)
		{
			oss << savedValue.mValueData;
		}
		else if (savedValue.type == SavedValue::VALUE_STRING)
		{
			oss << "\"" << savedValue.mStringData << "\"";
		}
		else if (savedValue.type == SavedValue::VALUE_BOOLEAN)
		{
			oss << (static_cast<bool>(savedValue.mValueData)) ? "true" : "false" ;
		}
		else
		{
			Debug::Warning("Invalid saved value type.");
		}
		oss << "\n";
	}
	const string& content = oss.str();
	FileData::SaveFile(mFileName, content);
}

void Savegame::SetInteger(const string & key, int value)
{
	SavedValue savedValue;
	savedValue.type = SavedValue::VALUE_INTEGER;
	savedValue.mValueData = value;

	mSavedValues[key] = savedValue;
}

int Savegame::GetInteger(const string & key) const
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != SavedValue::VALUE_INTEGER)
		{
			Debug::Warning("Excepted integer type in SaveGame::GetInteger.");
			return 0;
		}
		return it->second.mValueData;
	}
	return 0;
}

void Savegame::SetString(const string & key, const string & value)
{
	SavedValue savedValue;
	savedValue.type = SavedValue::VALUE_STRING;
	savedValue.mStringData = value;

	mSavedValues[key] = savedValue;
}

string Savegame::GetString(const string & key) const
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != SavedValue::VALUE_STRING)
		{
			Debug::Warning("Excepted integer type in SaveGame::GetInteger.");
			return "";
		}
		return it->second.mStringData;
	}
	return "";
}

void Savegame::SetBoolean(const string & key, bool value)
{
	SavedValue savedValue;
	savedValue.type = SavedValue::VALUE_BOOLEAN;
	savedValue.mValueData = static_cast<int>(value);

	mSavedValues[key] = savedValue;
}

bool Savegame::GetBoolean(const string & key)
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != SavedValue::VALUE_BOOLEAN)
		{
			Debug::Warning("Excepted integer type in SaveGame::GetInteger.");
			return false;
		}
		return static_cast<bool>(it->second.mValueData);
	}
	return false;
}

void Savegame::UnSet(const string & key)
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		mSavedValues.erase(it);
	}
}

const string Savegame::GetLuaObjectName() const
{
	return LuaContext::module_game_name;
}

int Savegame::LuaLoadConfig(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l , [&] {
	

		return 0;
	});
}

/**
*	\brief 从lua 文件中加载存档数据
*/
void Savegame::ImportFromFile(const string & filename)
{
	if (!FileData::IsFileExists(filename))
	{
		Debug::Error("The lua file " + filename + "not existed.");
		return;
	}

	const string buffer = FileData::ReadFile(filename);
	lua_State* l = luaL_newstate();
	if (luaL_loadbuffer(l, buffer.data(), buffer.size(), filename.c_str()) != 0)
	{
		Debug::Error("Can't load lua data file:" + filename);
		lua_pop(l, 1);
		return;
	}						// data
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "savegame");
							// data
	lua_newtable(l);
							// data env
	lua_newtable(l);
							// data env env_mt
	lua_pushcfunction(l, LuaLoadFunction);
							// data env evn_mt func
	lua_setfield(l, -2, "__newindex");
							// data env evn_mt 
	lua_setmetatable(l, -2);
							// data env
	lua_setupvalue(l, -2, 1);
							// data
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Failed to load config datafile.");
		lua_pop(l, 1);	// ?????
	}

	lua_close(l);
}

/**
*	\brief 设置默认初始值，仅在无存档文件时调用
*/
void Savegame::SetDefaultData()
{
}

/**
*	\brief 加载数据的C lua 函数
*/
int Savegame::LuaLoadFunction(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		lua_getfield(l, LUA_REGISTRYINDEX, "savegame");
		Savegame* savegame = static_cast<Savegame*>(lua_touserdata(l, -1));
		lua_pop(l, 1);

		const string& key = LuaTools::CheckString(l, 2);
		switch (lua_type(l, 3))
		{
		case LUA_TBOOLEAN:
			savegame->SetBoolean(key, LuaTools::CheckBoolean(l, 3));
			break;
		case LUA_TNUMBER:
			savegame->SetInteger(key, LuaTools::CheckInt(l, 3));
			break;
		case LUA_TSTRING:
			savegame->SetString(key, LuaTools::CheckString(l, 3));
			break;
		default:
			LuaTools::Error(l, "Invalid value.");
			break;
		}

		return 0;
	});
}
