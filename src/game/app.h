#ifndef _CJLING_APP_H_
#define _CJLING_APP_H_

#include"common\common.h"
#include"lua\luaContext.h"
#include"core\inputEvent.h"
#include"core\renderer.h"
#include"core\asyncLoader.h"
#include"game\sprite.h"
#include"game\animationSprite.h"
#include"game\game.h"
#include"entity\player.h"

// test
#include"gui\core\handler.h"
#include"gui\widget\window.h"
#include"gui\widget\button.h"
#include"core\sound.h"

/**
*	\brief 程序的主循环
*/
class App
{
public:
	App();
	~App();

	void Run();
	void Step();
	void Update();
	void SetExiting(bool isexit);

	void SetGame(Game* game);
	Game* GetGame();
	LuaContext& GetLuaContext();

private:
	bool IsExiting();
	void CheckInput();
	void Render();
	void NotifyInput(const InputEvent& ent);

	bool mExiting;				/** 是否退出 */

	std::unique_ptr<LuaContext> mLuaContext; /** 脚本管理者 */
	
	std::unique_ptr<Game> mCurrGame;		/** 当前游戏 */

	Game* mNextGame;			/** 下一个游戏，用于切换游戏 */
						
	std::unique_ptr<gui::GUIManager> mGUI;	

	std::shared_ptr<gui::Window> mWindow;

	Sound mSound;
};

#endif
