#include"game.h"
#include"core\inputEvent.h"
#include"game\app.h"
#include"game\savegame.h"
#include"game\map.h"
#include"game\gameCommands.h"
#include"game\equipment.h"
#include"lua\luaContext.h"

Game::Game(App* app):	// test,no savegame
	mStarted(false),
	mSavegame(nullptr),
	mPlayer(nullptr),
	mApp(*app),
	mGameCommands(nullptr),
	mCurrentMap(nullptr),
	mNextMap(nullptr)
{
	Equipment& equipment = mSavegame->GetEquipment();
	mPlayer = std::make_shared<Player>(equipment);
}

Game::Game(App* app, const std::shared_ptr<Savegame>& savegame):
	mStarted(false),
	mSavegame(savegame),
	mPlayer(nullptr),
	mApp(*app),
	mGameCommands(nullptr),
	mCurrentMap(nullptr),
	mNextMap(nullptr)
{
	savegame->SetGame(this);
	
	// ������Ϸ�������
	mGameCommands = std::make_shared<GameCommands>(*this);

	// ����player
	Equipment& equipment = mSavegame->GetEquipment();
	mPlayer = std::make_shared<Player>(equipment);

	// ����map
	string mapID = savegame->GetString(Savegame::KEYWORD_START_MAP);
	if (mapID.empty())
	{
		mapID = "test";
	}
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
*	\brief ��Ϸ�߼�����
*
*	���θ���map,equipment,HUD
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
*	\brief ��Ϸ����
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

std::shared_ptr<Player> Game::GetPlayer()
{
	return mPlayer;
}

GameCommands & Game::GetGameCommands()
{
	return *mGameCommands;
}

Equipment & Game::GetEquipment()
{
	Debug::CheckAssertion(mSavegame != nullptr,
		"The savegame must exists before get equipent.");
	return mSavegame->GetEquipment();
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
*	\brief ���õ�ǰ��ͼ
*
*	���õĵ�ͼ�Ļ���unpdateTransition�и���
*/
void Game::SetCurrentMap(const string & mapID)
{
	if (mCurrentMap == nullptr || mNextMap->GetMapID() != mCurrentMap->GetMapID())
	{
		// ������һ�ŵ�ͼ
		mNextMap = std::make_shared<Map>(mapID);
		mNextMap->Load(this);

		// ���ý�ɫ
		mPlayer->PlaceOnMap(*mNextMap);
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
	mGameCommands->NotifyInput(ent);
	return true;
}

void Game::NotifyGameCommandPressed(const GameCommand & gameCommand)
{
	// player����gameCommand,��������entity����lua��
	// ͨ���ű�����
	mPlayer->NotifyCommandPressed(gameCommand);
}

void Game::NotifyGameCommandReleased(const GameCommand & gameCommand)
{
	mPlayer->NotifyCommandReleased(gameCommand);
}

LuaContext& Game::GetLuaContext()
{
	return mApp.GetLuaContext();
}
