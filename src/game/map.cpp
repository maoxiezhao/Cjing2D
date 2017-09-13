#include "map.h"
#include "core\inputEvent.h"
#include "entity\entities.h"
#include "lua\luaContext.h"
#include "game\game.h"
#include "game\mapData.h"

Map::Map():
	mMapID(),
	mGame(nullptr),
	mWidth(0),
	mHeight(0),
	mMinLayer(0),
	mMaxLayer(0),
	mIsLoaded(false),
	mIsStarted(false),
	mCamera(nullptr),
	mTileset(nullptr),
	mEntities(nullptr)
{
}

Map::Map(const std::string& id):
	mMapID(id),
	mGame(nullptr),
	mWidth(0),
	mHeight(0),
	mMinLayer(0),
	mMaxLayer(0),
	mIsLoaded(false),
	mIsStarted(false),
	mCamera(nullptr),
	mTileset(nullptr),
	mEntities(nullptr)
{
}

Map::~Map()
{
}

/**
*	\brief ���ص�ͼ
*/
void Map::Load(Game * game)
{
	MapData mapData;
	const string& mapFileName = string("maps/") + GetMapID();
	bool successed = mapData.ImportFromFile(mapFileName);

	if (!successed)
	{
		Debug::Die("Failed to load map file '" + GetMapID() + "'.");
	}

	// ���ص�ͼ�ɹ������ʼ����ͼ����

	mEntities = nullptr;
	mTileset = nullptr;

	mGame = game;
	mIsLoaded = true;
}

/**
*	\brief ��ͼˢ��
*/
void Map::Update()
{
	CheckSuspended();

	DrawBackground();
	
	mEntities->Update();
	GetLuaContext().OnMapUpdate();

	DrawForeground();
}

bool Map::NotifyInput(const InputEvent & event)
{
	return false;
}

/**
*	\brief ����Ƿ���ͣ
*
*	map��suspended״̬��game����
*/
void Map::CheckSuspended()
{
	bool gameSuspended = mGame->IsSuspended();
	if (mSuspended !=  gameSuspended)
	{
		SetSuspended(gameSuspended);
	}
}

void Map::SetSuspended(bool suspended)
{
	mSuspended = suspended;
}

Game & Map::GetGame()
{
	return *mGame;
}

LuaContext & Map::GetLuaContext()
{
	return mGame->GetLuaContext();
}

const string Map::GetLuaObjectName() const
{
	return LuaContext::module_map_name;
}

const std::shared_ptr<Camera>& Map::GetCamera()
{
	return nullptr;
}

const string & Map::GetMapID() const
{
	return mMapID;
}

const Tileset & Map::GetTileset() const
{
	return *mTileset;
}

/**
*	\brief ���õ�ǰtileset
*/
void Map::SetTileset(const string & tilesetID)
{
	
}

const string & Map::getTilesetID() const
{
	return mTilesetId;
}

void Map::Draw()
{
	if (!IsLoaded())
	{
		return;
	}

	mEntities->Draw();
	GetLuaContext().OnMapDraw();
}

/**
*	\brief ���Ʊ���ͼ
*/
void Map::DrawBackground()
{
}

/**
*	\brief ����ǰ��ͼ
*/
void Map::DrawForeground()
{
}

bool Map::IsLoaded() const
{
	return mIsLoaded;
}

bool Map::IsStarted() const
{
	return mIsStarted;
}

bool Map::IsSuspended() const
{
	return mSuspended;
}
