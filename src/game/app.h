#ifndef _APP_H_
#define _APP_H_

#include"common\common.h"
#include"lua\luaContext.h"
#include"core\inputEvent.h"
#include"core\renderer.h"
#include"game\sprite.h"
#include"game\animationSprite.h"
#include"game\game.h"
/**
*	\brief 程序的主循环
*/
class App
{
public:
	App();
	~App();

	void Run();
	void Update();
	void SetExiting(bool isexit);
	void SetGame(Game* game);
	Game* GetGame();

private:
	bool IsExiting();
	void CheckInput();
	void Render();
	void NotifyInput(const InputEvent& ent);

	std::unique_ptr<LuaContext> mLuaContext;
	
	std::unique_ptr<Game> mCurrGame;   
	Game* mNextGame;

	bool mExiting;

	// test
	SpritePtr testSprite;
	AnimationSpritePtr testAnimation;
	
};

#endif
