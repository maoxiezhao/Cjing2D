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
*	\brief 加载地图
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

	// 加载地图成功，则初始化地图数据

	mEntities = nullptr;
	mTileset = nullptr;

	mGame = game;
	mIsLoaded = true;
}

/**
*	\brief 地图刷新
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
*	\brief 检查是否暂停
*
*	map的suspended状态由game控制
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
*	\brief 设置当前tileset
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
*	\brief 绘制背景图
*/
void Map::DrawBackground()
{
}

/**
*	\brief 绘制前景图
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
