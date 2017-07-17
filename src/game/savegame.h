#ifndef _SAVE_GAME_H_
#define _SAVE_GAME_H_

#include"common\common.h"
#include"game\game.h"

class Savegame
{
public:
	Savegame();

	void SetGame(Game* game);

private:
	Game* mGame;

};

#endif