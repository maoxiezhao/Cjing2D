#ifndef _GAME_H_
#define _GAME_H_

#include"common\common.h"
#include"game\gameCommands.h"

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
	
	/** System */
	void Start();
	void Stop();
	void Restart();
	void Update();
	void Draw();
	
	bool IsSuspended()const;
	Savegame& GetSavegame();
	std::shared_ptr<Player> GetPlayer();
	GameCommands& GetGameCommands();

	/** Map */
	bool HasCurrentMap()const;
	Map& GetCurrentMap();
	void SetCurrentMap(const string& mapID);
	void UpdateTransitoin();

	/** Notify */
	bool NotifyInput(const InputEvent & ent);
	void NotifyGameCommandPressed(const GameCommand& gameCommand);
	void NotifyGameCommandReleased(const GameCommand& gameCommand);

	/** Lua */
	LuaContext& GetLuaContext();

private:
	bool mStarted;
	bool mSuspended;

	App& mApp;

	std::shared_ptr<GameCommands> mGameCommands; /** 当前游戏的命令管理器 */

	std::shared_ptr<Savegame> mSavegame;	/** 当前游戏指向的游戏存档 */

	std::shared_ptr<Player> mPlayer;		/** 当前游戏的可控制对象 */

	std::shared_ptr<Map> mCurrentMap;		/** 当前游戏的地图 */

	std::shared_ptr<Map> mNextMap;			/** 当前游戏指向的下一张地图 */
};


#endif