#ifndef _SAVE_GAME_H_
#define _SAVE_GAME_H_

#include"common\common.h"
#include"game\game.h"
#include<map>

class Savegame
{
public:
	Savegame();

	void SetGame(Game* game);

	/** saved value operation */


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
	};

	std::map<std::string, SavedValue> mSavedValues;

private:
	Game* mGame;
};

#endif