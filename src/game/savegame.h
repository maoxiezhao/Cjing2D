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
	*	����ֵ�Ķ���ṹ�������˶����ֵ�Ͷ������������
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
	Equipment mEquipment;		/** ��ǰ�����״̬������ */
	string mFileName;			/** ��ǰ�浵�ļ��� */

};

#endif