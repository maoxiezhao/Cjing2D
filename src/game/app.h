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
#include"gui\widget\widget.h"
#include"gui\widget\grid.h"
#include"core\font.h"

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
					
	/** 用于异步加载资源 */
	void AsyncLoading();
	void LoadingFinishCallBack();
	AnimationSpritePtr mLoadingAnimate;	/** 加载时展示图片，当GUI实现后，考虑实现一个加载界面 */
	bool mAsyncLoaded;
	AsyncLoader mAsyncLoader;			

	/***  测试用声明 **/
	std::unique_ptr<gui::GUIManager> mGUI;
	std::shared_ptr<gui::Widget> mWidget1;
	std::shared_ptr<gui::Widget> mWidget2;
	std::shared_ptr<gui::Widget> mWidget3;
	std::shared_ptr<gui::Grid> mGrid;
	std::shared_ptr<font::Font> mFont;
};

#endif
