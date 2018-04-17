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
*/
class Savegame : public LuaObject
{
public:
	Savegame(App& app, const string& fileName);

	void Init();
	void SetGame(Game* game);
	Game* GetGame();
	void SaveGameToLocal();
	Equipment& GetEquipment();

	/** saved value operation */
	void SetInteger(const string& key, int value);
	int GetInteger(const string& key)const;
	void SetString(const string& key, const string& value);
	string GetString(const string& key)const;
	void SetBoolean(const string& key, bool value);
	bool GetBoolean(const string& key);
	void UnSet(const string& key);

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
	static const std::string KEYWORD_CURRENT_LIFE;
	static const std::string KEYWORD_CURRENT_MAX_LIFE;

private:
	/** lua newindex function */
	static int LuaLoadConfig(lua_State* l);
	static int LuaLoadFunction(lua_State* l);

	void ImportFromFile(const string& filename);
	void SetDefaultData();

	void SetDefaultCommandMappingKeyBoard();
	void SetDefualtCommandMappingMouse();
	void SetDefaultEquipmentState();

private:
	/**
	*	保存值的对象结构，包含了对象的值和对象的数据类型
	*/
	struct SavedValue
	{
		enum TYPE{
			VALUE_STRING,
			VALUE_INTEGER,
			VALUE_BOOLEAN
		};
		TYPE type;
		std::string mStringData;
		int mValueData;		    //	(Integer and boolean)

		SavedValue() :type(VALUE_STRING), mStringData(""), mValueData(0){}
	};

	std::map<std::string, SavedValue> mSavedValues;
	App& mApp;
	Game* mGame;
	Equipment mEquipment;		/** 当前的玩家状态管理者 */
	string mFileName;			/** 当前存档文件名 */

};

#endif