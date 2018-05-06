#ifndef _SAVE_GAME_H_
#define _SAVE_GAME_H_

#include"common\common.h"
#include"game\game.h"
#include"game\equipment.h"
#include"lua\luaObject.h"
#include<map>

class App;

/**
*	\brief ��Ϸ�浵����
*
*	��Ϸ�浵���ݹ�����player�����ݣ��Լ���ص��¼���Ϣ���ͻ�����Ϸ
*	���ã����簴���󶨣���Ļ�趨��
*
*	Ŀǰ�����󶨱��浽�ĵ��У�δ�����Ǳ���ɵ�����Input dat�ļ�
*
*	saveGame ��Ϊȫ�ֶ��󣬿�������洢lua table��ʽ 2018.3.15
*	TableValue������String����keyֵ��keyָ��һЩ������key����Щ
*	key�Ķ�ӦvalueҲ������savedValue��
*/
class Savegame : public LuaObject
{
public:
	/** �����Value���� */
	enum SAVED_VALUE_TYPE {
		VALUE_NONE = -1,
		VALUE_STRING = 0,
		VALUE_INTEGER,
		VALUE_BOOLEAN,
		VALUE_TABLE,
	};

	/** ����ֵ�Ķ���ṹ�������˶����ֵ�Ͷ������������ */
	struct SavedValue
	{
		SAVED_VALUE_TYPE type;
		std::string stringData;
		int valueData;		    //	(Integer and boolean)
		bool belongToTable;		// �Ƿ��Ǳ���value

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

	/*** commandӳ��洢�ı�� ***/
	static const std::string GAMECOMMAND_KEYBOARD_UP;
	static const std::string GAMECOMMAND_KEYBOARD_RIGHT;
	static const std::string GAMECOMMAND_KEYBOARD_DOWN;
	static const std::string GAMECOMMAND_KEYBOARD_LEFT;
	static const std::string GAMECOMMAND_KEYBOARD_INTERACT;
	static const std::string GAMECOMMAND_KEYBOARD_SHIFT;

	/*** *** ��ǰ��Ϸ��ֵ��Keyword *** ***/
	static const std::string KEYWORD_START_MAP;
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
	Equipment mEquipment;		/** ��ǰ�����״̬������ */
	string mFileName;			/** ��ǰ�浵�ļ��� */

	using ValueTable = std::vector<std::string>;
	std::map<std::string, ValueTable> mValueTables;
};

#endif