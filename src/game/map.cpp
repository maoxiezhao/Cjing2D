#include "map.h"
#include "lua\luaContext.h"
#include "core\inputEvent.h"
#include "game\game.h"
#include "game\mapData.h"
#include "game\sprite.h"
#include "entity\entities.h"
#include "entity\entity.h"
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
	mGame = game;
	mWidth  = mapData.GetSize().width;
	mHeight = mapData.GetSize().height;
	mMinLayer = mapData.GetMinLayer();
	mMaxLayer = mapData.GetMaxLayer();
	mTilesetId = mapData.getTitlesetID();
	mTileset = std::make_shared<Tileset>(mTilesetId);
	mTileset->Load();

	mEntities = std::unique_ptr<Entities>(new Entities(*game, *this));
	mEntities->InitEntities(mapData);
	
	mCamera = mEntities->GetCamear();
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

Ground Map::GetGround(int layer, int x, int y)
{
	return Ground::GROUND_EMPTY;
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

std::shared_ptr<Camera>& Map::GetCamera()
{
	return mCamera;
}

const std::shared_ptr<Camera>& Map::GetCamera() const
{
	return mCamera;
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

Size Map::GetSize() const
{
	return Size(mWidth, mHeight);
}

int Map::GetWidth()const
{
	return mWidth;
}

int Map::GetHeight()const
{
	return mHeight;
}

/**
*	\brief 测试entity与障碍物的碰撞检测
*	\param rect 测试entity的包围盒rect
*	\param entity 参与碰撞检测的entity
*	\return true 发生了碰撞检测
*/
bool Map::TestCollisionWithObstacle(const Rect & rect, Entity & entity)
{
	// 与map当前layerd的ground
	int xBegin = rect.x;
	int xEnd = xBegin + rect.width - 1;
	int yBegin = rect.y;
	int yEnd = yBegin + rect.height - 1;
	int layer = entity.GetLayer();

	// 仅计算4条边界即可
	for (int x = xBegin; x <= xEnd; x += 8)
	{
		if (TestCollisionWithGround(layer, x, yBegin, entity) ||
			TestCollisionWithGround(layer, x, yEnd, entity) ||
			TestCollisionWithGround(layer, x + 7, yBegin, entity) ||
			TestCollisionWithGround(layer, x + 7, yEnd, entity)){
			return true;
		}
	}

	// 测试以每8个像素为间距的点的ground是否是障碍点
	for (int y = yBegin; y <= yEnd; y += groundCellHeight)
	{
		if (TestCollisionWithGround(layer, xBegin, y, entity) ||
			TestCollisionWithGround(layer, xEnd,   y, entity) ||
			TestCollisionWithGround(layer, xBegin, y + 7, entity) ||
			TestCollisionWithGround(layer, xEnd, y + 7, entity)) {
			return true;
		}
	}
	return false;
}

bool Map::TestCollisionWithGround(int layer, int x, int y, Entity & entity)
{
	bool isObstacle = false;
	Ground ground = mEntities->GetGround(layer, x / groundCellWidth, y / groundCellHeight);
	switch (ground)
	{
	case GROUND_EMPTY:
		isObstacle = false;
		break;
	case GROUND_WALL:
		isObstacle = true;
		break;
	}

	return isObstacle;
}

/**
*	\brief 检测实体与其他实体的碰撞
*
*	当其他实体可碰撞时且发生碰撞时，会触发notifyCollision
*/
void Map::CheckCollisionWithEntities(Entity & entity)
{
	Rect checkRect = entity.GetRectBounding();
	checkRect.Extend(8, 8);

	// 获取临近范围内的entity
	std::vector<EntityPtr> entityNearby;
	mEntities->GetEntitiesInRect(checkRect, entityNearby);
	for (auto& checkEntity : entityNearby)
	{
		if (!checkEntity->IsHaveCollision() || 
			checkEntity.get() == &entity)
		{
			continue;
		}
		checkEntity->CheckCollision(entity);
	}
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

/**
*	\brief 在地图上绘制drawable
*/
void Map::DrawOnMap(Drawable & drawabel)
{
	if (mCamera == nullptr)
	{
		Debug::Warning("Draw drawable without camear.");
		return;
	}
	const Point2& pos = drawabel.GetPos();
	drawabel.Draw(pos - mCamera->GetLeftTopPos());
}

/**
*	\brief 在地图上绘制drawable
*/
void Map::DrawOnMap(Drawable& drawabel, const Point2& pos)
{
	if (mCamera == nullptr)
	{
		Debug::Warning("Draw drawable without camear.");
		return;
	}
	drawabel.Draw(pos - mCamera->GetLeftTopPos());
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
