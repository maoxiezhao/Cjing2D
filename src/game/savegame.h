#ifndef _SAVE_GAME_H_
#define _SAVE_GAME_H_

#include"common\common.h"
#include"game\game.h"
#include"game\equipment.h"
#include"lua\luaObject.h"
#include<map>

class App;

/**
*	\brief 游戏存档数据
*
*	游戏存档数据管理着player的数据，以及相关的事件信息，和基本游戏
*	设置（例如按键绑定，屏幕设定）
*
*	目前按键绑定保存到文档中，未来考虑保存成单独的Input dat文件
*
*	saveGame 作为全局对象，可以任意存储lua table格式 2018.3.15
*	TableValue仅仅在String保存key值，key指向一些列其他key，这些
*	key的对应value也保存在savedValue中
*/
class Savegame : public LuaObject
{
public:
	/** 保存的Value类型 */
	enum SAVED_VALUE_TYPE {
		VALUE_NONE = -1,
		VALUE_STRING = 0,
		VALUE_INTEGER,
		VALUE_BOOLEAN,
		VALUE_TABLE,
	};

	/** 保存值的对象结构，包含了对象的值和对象的数据类型 */
	struct SavedValue
	{
		SAVED_VALUE_TYPE type;
		std::string stringData;
		int valueData;		    //	(Integer and boolean)
		bool belongToTable;		// 是否是表中value

		SavedValue() :
			type(VALUE_STRING), 
			stringData(""),
			valueData(0),
			belongToTable(false){}
	};

public:
	Savegame(App& app, const string& fileName);

	void Init();
	void SetGame(Game* game);
	Game* GetGame();
	void SaveGameToLocal();
	Equipment& GetEquipment();

	/** saved value operation */
	void SetInteger(const string& key, int value, const std::string& tableKey = "");
	int GetInteger(const string& key)const;
	void SetString(const string& key, const string& value, const std::string& tableKey = "");
	string GetString(const string& key)const;
	void SetBoolean(const string& key, bool value, const std::string& tableKey = "");
	bool GetBoolean(const string& key);
	void UnSet(const string& key);
	void SetTable(const string& key, const std::string& tableKey, const std::string& baseTableKey = "");
	void InsertToTable(const std::string& tableKey, const std::string& key, SavedValue& value);
	std::vector<std::string> GetTable(const std::string& key);

	SAVED_VALUE_TYPE GetValueType(const std::string& key);
	LuaContext& GetLuaContext();
	virtual const string GetLuaObjectName()const;

	/*** command映射存储的标记 ***/
	static const std::string GAMECOMMAND_KEYBOARD_UP;
	static const std::string GAMECOMMAND_KEYBOARD_RIGHT;
	static const std::string GAMECOMMAND_KEYBOARD_DOWN;
	static const std::string GAMECOMMAND_KEYBOARD_LEFT;
	static const std::string GAMECOMMAND_KEYBOARD_INTERACT;
	static const std::string GAMECOMMAND_KEYBOARD_SHIFT;

	/*** *** 当前游戏数值的Keyword *** ***/
	static const std::string KEYWORD_START_MAP;
	static const std::string KEYWORD_START_DESTINATION;
	static const std::string KEYWORD_CURRENT_LIFE;
	static const std::string KEYWORD_CURRENT_MAX_LIFE;

	static const std::string PLAYER_EQUIP_WEAPON;

	static const std::string EQUIPMENT_ALL_ITEM;
	static const std::string EQUIPMENT_EQUIP_WEAPON;

private:
	/** lua newindex function */
	static int LuaLoadConfig(lua_State* l);
	static int LuaLoadFunction(lua_State* l);
	static int LuaLoadValueFunction(lua_State* l, Savegame& savegame, int index, const std::string& tableKey = "");

	void ImportFromFile(const string& filename);
	void SetDefaultData();

	void SetDefaultCommandMappingKeyBoard();
	void SetDefualtCommandMappingMouse();
	void SetDefaultEquipmentState();

	void ExportSaveValue(const std::string& key, const SavedValue& value, std::ostringstream& oss);

private:
	std::map<std::string, SavedValue> mSavedValues;
	App& mApp;
	Game* mGame;
	Equipment mEquipment;		/** 当前的玩家状态管理者 */
	string mFileName;			/** 当前存档文件名 */

	using ValueTable = std::vector<std::string>;
	std::map<std::string, ValueTable> mValueTables;
};

#endif