#include "map.h"
#include "core\inputEvent.h"
#include "entity\entities.h"
#include "lua\luaContext.h"
#include "game\game.h"
#include "game\mapData.h"
#include "game\sprite.h"
#include "entity\tileset.h"
#include "entity\tilepattern.h"

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
*
*	地图数据包括.dat数据和.lua脚本，.dat为地图所有entity数据
*	在load时会加载，而.lua脚本则在start时从luaContext中加载
*/
void Map::Load(Game * game)
{
	MapData mapData;
	const string& mapFileName = string("maps/") + GetMapID() + ".dat";
	bool successed = mapData.ImportFromFile(mapFileName);
	if (!successed)
	{
		Debug::Die("Failed to load map file '" + GetMapID() + "'.");
	}
	// 加载地图成功，则初始化地图数据
	mWidth  = mapData.GetSize().width;
	mHeight = mapData.GetSize().height;
	mMinLayer = mapData.GetMinLayer();
	mMaxLayer = mapData.GetMaxLayer();
	mTilesetId = mapData.getTitlesetID();
	mTileset = std::make_shared<Tileset>(mTilesetId);
	mTileset->Load();

	mEntities = std::unique_ptr<Entities>(new Entities(*game, *this));
	mEntities->InitEntities(mapData);

	mGame = game;
	mIsLoaded = true;
}

void Map::UnLoad()
{
	if (mTileset != nullptr)
	{
		mTileset->UnLoad();
	}
}

/**
*	\brief 开始地图,执行lua的runMap方法
*/
void Map::Start()
{
	mIsStarted = true;
	mEntities->NotifyMapStarted();
	GetLuaContext().RunMap(*this);
}

/**
*	\brief 离开地图
*/
void Map::Leave()
{
	mIsStarted = false;
}

/**
*	\brief 地图刷新
*/
void Map::Update()
{
	CheckSuspended();

	// 更新entity
	mEntities->Update();

	// 更新tilePattern
	TilePattern::Update();

	GetLuaContext().OnMapUpdate(*this);
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

/**
*	\brief 是否是合法的layer
*/
bool Map::IsValidLayer(int layer) const
{
	return (layer >= mMinLayer && 
		layer <= mMaxLayer);
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

Entities & Map::GetEntities()
{
	return *mEntities;
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
	mTilesetId = tilesetID;
}

const string & Map::getTilesetID() const
{
	return mTilesetId;
}

int Map::GetMinLayer() const
{
	return mMinLayer;
}

int Map::GetMaxLayer() const
{
	return mMaxLayer;
}

/**
*	\brief 绘制地图
*
*	绘制过程依次包括背景图，entities,前景图,luaContent
*/
void Map::Draw()
{
	if (!IsLoaded())
	{
		return;
	}
	DrawBackground();
	mEntities->Draw();
	DrawForeground();

	// test
	//mTileset->GetTileImage()->SetPos({ 0, 0 });
	//mTileset->GetTileImage()->SetTextureRect(Rect(0, 0, 16, 16), true);
 //   mTileset->GetTileImage()->Draw();
	//
	//mTileset->GetTileImage()->SetPos({ 16, 0 });
	//mTileset->GetTileImage()->SetTextureRect(Rect(16, 0, 32, 16), true);
	//mTileset->GetTileImage()->Draw();

	GetLuaContext().OnMapDraw(*this);
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
