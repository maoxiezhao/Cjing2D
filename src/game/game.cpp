#include"game.h"
#include"core\inputEvent.h"
#include"game\app.h"
#include"game\savegame.h"
#include"game\map.h"
#include"lua\luaContext.h"

Game::Game(App* app):	// test,no savegame
	mStarted(false),
	mSavegame(nullptr),
	mPlayer(nullptr),
	mApp(*app),
	mCurrentMap(nullptr),
	mNextMap(nullptr)
{
	mPlayer = std::make_shared<Player>();
}

Game::Game(App* app, const std::shared_ptr<Savegame>& savegame):
	mStarted(false),
	mSavegame(savegame),
	mPlayer(nullptr),
	mApp(*app),
	mCurrentMap(nullptr),
	mNextMap(nullptr)
{
	savegame->SetGame(this);

	// 加载player
	// mCommands = new Commands();
	mPlayer = std::make_shared<Player>();

	// 加载map,now for testing.
	string mapID = "test";      //savegame->getStringMapId();
	string destination_name = ""; //savegame->getStringDestination();
	
	SetCurrentMap(mapID);
}

void Game::Start()
{
	if (mStarted)
	{
		return;
	}

	mStarted = true;
	GetLuaContext().OnGameStart(*this);
}

void Game::Stop()
{
	if (!mStarted)
	{
		return;
	}
	if (mCurrentMap != nullptr)
	{
		mCurrentMap->UnLoad();
	}

	mStarted = false;
	GetLuaContext().OnGameFinish(*this);
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
	GetLuaContext().OnGameUpdate(*this);
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
		GetLuaContext().OnGameDraw(*this);
	}
}

bool Game::IsSuspended() const
{
	return mSuspended;
}

Savegame & Game::GetSavegame()
{
	Debug::CheckAssertion(mSavegame != nullptr, "Invalid game without savegame.");
	return *mSavegame;
}

bool Game::HasCurrentMap() const
{
	return false;
}

Map & Game::GetCurrentMap()
{
	return *mCurrentMap;
}

/**
*	\brief 设置当前地图
*
*	设置的地图的会在unpdateTransition中更新
*/
void Game::SetCurrentMap(const string & mapID)
{
	if (mCurrentMap == nullptr || mNextMap->GetMapID() != mCurrentMap->GetMapID())
	{
		mNextMap = std::make_shared<Map>(mapID);
		mNextMap->Load(this);
	}
	else
	{
		mNextMap = mCurrentMap;
	}
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
		// start game
		mCurrentMap->Start();
	}
}

bool Game::NotifyInput(const InputEvent & ent)
{
	if (mCurrentMap != nullptr)
	{
		bool handle = GetLuaContext().OnGameInput(*this, ent);
		if (!handle)
		{
			//handle = mCurrentMap->NotifyInput(ent);
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
