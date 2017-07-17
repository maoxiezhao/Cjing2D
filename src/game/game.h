#ifndef _GAME_H_
#define _GAME_H_

#include"common\common.h"

class Map;
class Player;
class App;
class InputEvent;
class Savegame;
/**
*	\brief Game类管理当前地图，所有的游戏对象
*/
class Game
{
public:
	Game(App* app);	// test,no savegame
	Game(App* app, const std::shared_ptr<Savegame> savegame);
	
	Game(const Game& game) = delete;
	Game& operator=(const Game& game) = delete;
	
	// status
	void Start();
	void Stop();
	void Restart();
	void Update();
	void Draw();

	// map
	bool HasCurrentMap()const;
	Map& GetCurrentMap();
	void SetCurrentMap(const string& mapID);

	// notify
	void NotifyInput(const InputEvent & ent);

private:
	bool mStarted;

	std::shared_ptr<Player> mPlayer;

	std::shared_ptr<Map> mCurrentMap;
	std::shared_ptr<Map> mNextMap;

};


#endif