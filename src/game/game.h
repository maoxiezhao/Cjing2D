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
*	\brief Game�����ǰ��ͼ�����е���Ϸ����
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

	std::shared_ptr<GameCommands> mGameCommands; /** ��ǰ��Ϸ����������� */

	std::shared_ptr<Savegame> mSavegame;	/** ��ǰ��Ϸָ�����Ϸ�浵 */

	std::shared_ptr<Player> mPlayer;		/** ��ǰ��Ϸ�Ŀɿ��ƶ��� */

	std::shared_ptr<Map> mCurrentMap;		/** ��ǰ��Ϸ�ĵ�ͼ */

	std::shared_ptr<Map> mNextMap;			/** ��ǰ��Ϸָ�����һ�ŵ�ͼ */
};


#endif