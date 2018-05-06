#include"game\savegame.h"
#include"game\enumInfo.h"
#include"game\app.h"
#include"core\fileData.h"
#include"core\logger.h"
#include"lua\luaContext.h"

#include<sstream>

const std::string Savegame::GAMECOMMAND_KEYBOARD_UP = "keyboard_up";
const std::string Savegame::GAMECOMMAND_KEYBOARD_RIGHT = "keyboard_right";
const std::string Savegame::GAMECOMMAND_KEYBOARD_DOWN = "keyboard_down";
const std::string Savegame::GAMECOMMAND_KEYBOARD_LEFT = "keyboard_left";
const std::string Savegame::GAMECOMMAND_KEYBOARD_INTERACT = "keyboard_interact";
const std::string Savegame::GAMECOMMAND_KEYBOARD_SHIFT = "keyboard_shift";

const std::string Savegame::KEYWORD_START_MAP = "keyword_start_map";
const std::string Savegame::KEYWORD_CURRENT_LIFE = "keyword_current_life";
const std::string Savegame::KEYWORD_CURRENT_MAX_LIFE = "keyword_current_max_life";

const std::string Savegame::PLAYER_EQUIP_WEAPON = "player_equip_weapon";

const std::string Savegame::EQUIPMENT_ALL_ITEM = "equipment_all_items";
const std::string Savegame::EQUIPMENT_EQUIP_WEAPON = "equipment_equip_weapons";

Savegame::Savegame(App & app, const string & fileName):
	mApp(app),
	mFileName(fileName),
	mGame(nullptr),
	mEquipment(*this)
{
}

/**
*	\brief savegame 初始化
*
*	该函数在savegame load时调用
*/
void Savegame::Init()
{
	// 初始化一些相关的环境
	SetTable(EQUIPMENT_ALL_ITEM, EQUIPMENT_ALL_ITEM);
	SetTable(EQUIPMENT_EQUIP_WEAPON, EQUIPMENT_EQUIP_WEAPON);

	// 加载或创建自定义数据
	if (!FileData::IsFileExists(mFileName))
		SetDefaultData();
	else
		ImportFromFile(mFileName);

	mEquipment.LoadGame();
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
	Game* game = GetGame();
	Debug::CheckAssertion(game != nullptr, "Save game without game instance.");

	if (!mEquipment.SaveGame())
	{
		Debug::Warning("Failed to save game");
		return;
	}

	/** export game data */
	std::ostringstream oss;
	for (const auto& kvp : mSavedValues)
	{
		const string& key = kvp.first;
		auto& savedValue = kvp.second;	
		if(!savedValue.belongToTable)
			ExportSaveValue(key, savedValue, oss);
	}

	const string& content = oss.str();
	FileData::SaveFile(mFileName, content);
	Logger::Info("Save game succeed.");
}

Equipment & Savegame::GetEquipment()
{
	return mEquipment;
}

void Savegame::SetInteger(const string & key, int value, const std::string& tableKey)
{
	SavedValue savedValue;
	savedValue.type = Savegame::VALUE_INTEGER;
	savedValue.valueData = value;

	if (tableKey != "")
		InsertToTable(tableKey, key, savedValue);

	mSavedValues[key] = savedValue;
}

int Savegame::GetInteger(const string & key) const
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != Savegame::VALUE_INTEGER)
		{
			Debug::Warning("Excepted integer type in SaveGame::GetInteger.");
			return 0;
		}
		return it->second.valueData;
	}
	return 0;
}

void Savegame::SetString(const string & key, const string & value, const std::string& tableKey)
{
	SavedValue savedValue;
	savedValue.type = Savegame::VALUE_STRING;
	savedValue.stringData = value;

	if (tableKey != "")
		InsertToTable(tableKey, key, savedValue);

	mSavedValues[key] = savedValue;
}

string Savegame::GetString(const string & key) const
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != Savegame::VALUE_STRING)
		{
			Debug::Warning("Excepted integer type in SaveGame::GetInteger.");
			return "";
		}
		return it->second.stringData;
	}
	return "";
}

void Savegame::SetBoolean(const string & key, bool value, const std::string& tableKey)
{
	SavedValue savedValue;
	savedValue.type = Savegame::VALUE_BOOLEAN;
	savedValue.valueData = static_cast<int>(value);

	if (tableKey != "")
		InsertToTable(tableKey, key, savedValue);

	mSavedValues[key] = savedValue;
}

bool Savegame::GetBoolean(const string & key)
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != Savegame::VALUE_BOOLEAN)
		{
			Debug::Warning("Excepted integer type in SaveGame::GetInteger.");
			return false;
		}
		return static_cast<bool>(it->second.valueData);
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

/**
*	\brief 创建一个TableValue
*	\param tableKey 表的名字
*	\param baseTableKey 所属的表的名字，如果为"",则不属于任何表
*/
void Savegame::SetTable(const string & key, const std::string& tableKey, const std::string& baseTableKey)
{
	SavedValue savedValue;
	savedValue.type = Savegame::VALUE_TABLE;
	savedValue.stringData = tableKey;
	mValueTables[tableKey] = {};

	if (baseTableKey != "")
		InsertToTable(baseTableKey, key, savedValue);

	mSavedValues[key] = savedValue;
}

void Savegame::InsertToTable(const std::string & tableKey, const std::string & key, SavedValue & value)
{
	auto it = mValueTables.find(tableKey);
	Debug::CheckAssertion(it != mValueTables.end(), "The value table '"
		+ tableKey + "' is non-exsist");

	value.belongToTable = true;

	auto& tableKeys = it->second;	// 是否可以每次插入前都新设置表，这样无需这里判断
	if(std::find(tableKeys.begin(), tableKeys.end(), key) == tableKeys.end())
		it->second.push_back(key);
}

std::vector<std::string> Savegame::GetTable(const std::string & key)
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		if (it->second.type != Savegame::VALUE_TABLE)
		{
			Debug::Warning("Excepted table type in SaveGame::GetTable.");
			return {};
		}	
		auto kvp = mValueTables.find(it->second.stringData);
		if (kvp == mValueTables.end())
		{
			Debug::Warning("The table '" + key + "' is empty");
			return {};
		}
		return kvp->second;
	}
	return {};
}

Savegame::SAVED_VALUE_TYPE Savegame::GetValueType(const std::string & key)
{
	auto it = mSavedValues.find(key);
	if (it != mSavedValues.end())
	{
		return it->second.type;
	}
	return Savegame::VALUE_NONE;
}

LuaContext& Savegame::GetLuaContext()
{
	return mApp.GetLuaContext();
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
*
*	依次设置默认的输入映射
*/
void Savegame::SetDefaultData()
{
	SetDefaultCommandMappingKeyBoard();
	SetDefualtCommandMappingMouse();
	SetDefaultEquipmentState();
}

/**
*	\brief 设置默认的键盘映射
*/
void Savegame::SetDefaultCommandMappingKeyBoard()
{
	SetString(GAMECOMMAND_KEYBOARD_UP, EnumToString(InputEvent::KEY_w));
	SetString(GAMECOMMAND_KEYBOARD_RIGHT, EnumToString(InputEvent::KEY_d));
	SetString(GAMECOMMAND_KEYBOARD_DOWN, EnumToString(InputEvent::KEY_s));
	SetString(GAMECOMMAND_KEYBOARD_LEFT, EnumToString(InputEvent::KEY_a));
	SetString(GAMECOMMAND_KEYBOARD_INTERACT, EnumToString(InputEvent::KEY_e));
	SetString(GAMECOMMAND_KEYBOARD_SHIFT, EnumToString(InputEvent::KEY_SPACE));
}

/**
*	\brief 设置默认的鼠标映射
*/
void Savegame::SetDefualtCommandMappingMouse()
{
}

/**
*	\brief 设置默认的equipment状态
*/
void Savegame::SetDefaultEquipmentState()
{
	mEquipment.SetLife(10);
	mEquipment.SetMaxLife(10);
}

void Savegame::ExportSaveValue(const std::string & key, const SavedValue & value, std::ostringstream& oss)
{
	oss << key << " = ";
	if (value.type == Savegame::VALUE_INTEGER)
	{
		oss << value.valueData;
	}
	else if (value.type == Savegame::VALUE_STRING)
	{
		oss << "\"" << value.stringData << "\"";
	}
	else if (value.type == Savegame::VALUE_BOOLEAN)
	{
		oss << ((value.valueData != 0) ? "true" : "false");
	}
	else if (value.type == Savegame::VALUE_TABLE)
	{
		oss << "{" << "\n";	
		const std::string& key = value.stringData;
		auto kvp = mValueTables.find(key);
		if (kvp != mValueTables.end())
		{
			auto& valueTable = kvp->second;
			for (const auto& k : valueTable)
			{
				// 必须保证每个key都对应一个savedValue
				const auto& v = mSavedValues[k];
				ExportSaveValue(k, v, oss);
				oss << ",";
			}
		}
		oss << "}";
	}
	else
	{
		Debug::Warning("Invalid saved value type.");
	}
	oss << "\n";
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

		LuaLoadValueFunction(l, *savegame, 2);
		return 0;
	});
}

int Savegame::LuaLoadValueFunction(lua_State * l, Savegame& savegame, int index, const std::string& tableKey)
{
	const string& key = LuaTools::CheckString(l, index);
	int valueIndex = index + 1;
	switch (lua_type(l, valueIndex))
	{
	case LUA_TBOOLEAN:
		savegame.SetBoolean(key, LuaTools::CheckBoolean(l, valueIndex), tableKey);
		break;
	case LUA_TNUMBER:
		savegame.SetInteger(key, LuaTools::CheckInt(l, valueIndex), tableKey);
		break;
	case LUA_TSTRING:
		savegame.SetString(key, LuaTools::CheckString(l, valueIndex), tableKey);
		break;
	case LUA_TTABLE:
		savegame.SetTable(key, key, tableKey);
		lua_pushnil(l);	// key table 
		while (lua_next(l, -2))
		{				// key table key value
			int keyIndex = LuaTools::GetPositiveIndex(l, -2);
			LuaLoadValueFunction(l, savegame, keyIndex, key);
			lua_pop(l, 1);
		}
		break;
	default:
		LuaTools::Error(l, "Invalid value.");
		break;
	}
	return 0;
}
