#ifndef _SAVE_GAME_H_
#define _SAVE_GAME_H_

#include"common\common.h"
#include"game\game.h"
#include"lua\luaObject.h"
#include<map>

class Savegame : public LuaObject
{
public:
	Savegame(const string& fileName);

	void Init();
	void SetGame(Game* game);
	Game* GetGame();
	void SaveGameToLocal();

	/** saved value operation */
	void SetInteger(const string& key, int value);
	int GetInteger(const string& key)const;
	void SetString(const string& key, const string& value);
	string GetString(const string& key)const;
	void SetBoolean(const string& key, bool value);
	bool GetBoolean(const string& key);

	virtual const string GetLuaObjectName()const;

private:
	struct SavedValue
	{
		enum TYPE{
			VALUE_STRING,
			VALUE_INTEGER,
			VALUE_BOOLEAN
		};
		TYPE type;
		std::string mStringData;
		int mValueData;		//	(Integer and boolean)

		SavedValue() :type(VALUE_STRING), mStringData(""), mValueData(0){}
	};

	std::map<std::string, SavedValue> mSavedValues;
	Game* mGame;
	string mFileName;

};

#endif