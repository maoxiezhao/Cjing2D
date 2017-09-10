#include"savegame.h"
#include"lua\luaContext.h"
#include"core\fileData.h"
#include<sstream>

Savegame::Savegame(const string& fileName):
	mFileName(fileName)
{
}

void Savegame::Init()
{
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
	for (auto kvp : mSavedValues)
	{
		auto& savedValue = kvp.second;
		oss << kvp.first << " = ";
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
		oss << std::endl;
	}

	FileData::SaveFile(mFileName, oss.str());
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

const string Savegame::GetLuaObjectName() const
{
	return LuaContext::module_game_name;
}
