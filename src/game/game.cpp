#include"game.h"
#include"core\inputEvent.h"
#include"game\app.h"
#include"game\savegame.h"
#include"game\map.h"
#include"lua\luaContext.h"

Game::Game(App* app):	// test,no savegame
	mStarted(false),
	mApp(*app),
	mCurrentMap(nullptr),
	mNextMap(nullptr)
{
	mPlayer = std::make_shared<Player>();
}

Game::Game(App* app, const std::shared_ptr<Savegame>& savegame):
	mStarted(false),
	mApp(*app),
	mCurrentMap(nullptr),
	mNextMap(nullptr)
{
	savegame->SetGame(this);

	// 加载player
	// mCommands = new Commands();
	mPlayer = std::make_shared<Player>();

	// 加载map
	string mapID;
	// string map_id = savegame->getStringMapId();
	// string destination_name = savegame->getStringDestination();
	
	SetCurrentMap(mapID);
}

void Game::Start()
{
	if (mStarted)
	{
		return;
	}

	mStarted = true;
	GetLuaContext().OnGameStart();
}

void Game::Stop()
{
	if (!mStarted)
	{
		return;
	}

	mStarted = false;
	GetLuaContext().OnGameFinish();
}

void Game::Restart()
{
}

/**
*	\brief 游戏逻辑更新
*
*	依次更新map,equipment,HUD
*/
void Game::Update()
{
	UpdateTransitoin();

	if (!mStarted)
	{
		return;
	}
	mCurrentMap->Update();	
	GetLuaContext().OnGameUpdate();
}

/**
*	\brief 游戏绘制
*/
void Game::Draw()
{
	if (mCurrentMap == nullptr)
	{
		return;
	}
	if (mCurrentMap != nullptr)
	{
		mCurrentMap->Draw();
		GetLuaContext().OnGameDraw();
	}
}

bool Game::IsSuspended() const
{
	return mSuspended;
}

bool Game::HasCurrentMap() const
{
	return false;
}

Map & Game::GetCurrentMap()
{
	return *mCurrentMap;
}

void Game::SetCurrentMap(const string & mapID)
{
	auto newMap = std::make_shared<Map>();
	mNextMap = newMap;	
}

void Game::UpdateTransitoin()
{
	if (mNextMap != nullptr)
	{
		if (mCurrentMap == nullptr)
		{
			mCurrentMap = mNextMap;
			mNextMap = nullptr;
		}
	}
}

bool Game::NotifyInput(const InputEvent & ent)
{
	if (mCurrentMap != nullptr)
	{
		bool handle = GetLuaContext().OnGameInput(ent);
		if (!handle)
		{
			handle = mCurrentMap->NotifyInput(ent);
			if (!handle)
			{

			}
		}
	}
	return true;
}

LuaContext& Game::GetLuaContext()
{
	return mApp.GetLuaContext();
}
