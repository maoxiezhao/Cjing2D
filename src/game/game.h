#ifndef _GAME_H_
#define _GAME_H_

#include"common\common.h"

class Map;
class Player;
class App;
class InputEvent;
class Savegame;
class LuaContext;
/**
*	\brief Game类管理当前地图，所有的游戏对象
*/
class Game
{
public:
	Game(App* app);	// test,no savegame
	Game(App* app, const std::shared_ptr<Savegame>& savegame);
	
	Game(const Game& game) = delete;
	Game& operator=(const Game& game) = delete;
	
	// system
	void Start();
	void Stop();
	void Restart();
	void Update();
	void Draw();
	
	bool IsSuspended()const;

	// map
	bool HasCurrentMap()const;
	Map& GetCurrentMap();
	void SetCurrentMap(const string& mapID);
	void UpdateTransitoin();

	// notify
	bool NotifyInput(const InputEvent & ent);

	// lua
	LuaContext& GetLuaContext();


private:
	bool mStarted;
	bool mSuspended;

	App& mApp;

	std::shared_ptr<Player> mPlayer;

	std::shared_ptr<Map> mCurrentMap;
	std::shared_ptr<Map> mNextMap;

};


#endif