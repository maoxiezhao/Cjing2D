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
#include "entity\destination.h"

Map::Map():
	mMapID(),
	mGame(nullptr),
	mWidth(0),
	mHeight(0),
	mMinLayer(0),
	mMaxLayer(0),
	mSuspended(false),
	mIsLoaded(false),
	mIsStarted(false),
	mCamera(nullptr),
	mTileset(nullptr),
	mEntities(nullptr),
	mBackGround(nullptr),
	mFrontGround(nullptr)
{
}

Map::Map(const std::string& id):
	mMapID(id),
	mGame(nullptr),
	mWidth(0),
	mHeight(0),
	mMinLayer(0),
	mMaxLayer(0),
	mSuspended(false),
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
	// 加载地图成功，则初始化地图数据
	mGame = game;

	// debug map load
	if (!mMapGenerate.LoadMap(GetMapID()))
	{
		Debug::Die("Failed to load map file '" + GetMapID());
		return;
	}

	// map base property
	mWidth    = mMapGenerate.GetSize().width;
	mHeight   = mMapGenerate.GetSize().height;
	mMinLayer = mMapGenerate.GetMinLayer();
	mMaxLayer = mMapGenerate.GetMaxLayer();
	mTilesetId = mMapGenerate.getTitlesetID();
	mTileset = std::make_shared<Tileset>(mTilesetId);
	mTileset->Load();
	mEntities = std::unique_ptr<Entities>(new Entities(*game, *this));

	// 加载房间数据
	auto& rooms = mMapGenerate.GetMapRoomIDs();
	for( auto& id : rooms)
	{
		auto rects = mMapGenerate.GetMapRoomRects(id);
		auto& mapData = mMapGenerate.GetMapData(id);
		for (int i = 0; i < rects.size(); i++)
		{
			std::string roomName = id + "_" + std::to_string(i);
			mEntities->InitEntities(*mapData, rects[i].GetPos(), roomName);
		}
	}

	// 加载走廊数据
	auto& hallways = mMapGenerate.GetHallwayDatas();
	for (auto& hallway : hallways)
	{
		auto& mapData = mMapGenerate.GetHallwayMapData(hallway.type);
		mEntities->InitEntities(mapData, hallway.pos, "");
	}

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
	mEntities->NotifyMapStoped();
	GetLuaContext().LeaveMap(*this);
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

LuaContext& Map::GetLuaContext()
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

/**
*	\brief 获取范围Rect内的所有障碍物
*/
std::vector<EntityPtr> Map::GetObstacles(const Rect & rect, Entity & src) const
{
	std::vector<EntityPtr> obstacles;	
	int srcLayer = src.GetLayer();

	// 没必要用两个vector
	std::vector<EntityPtr> temps;
	mEntities->GetEntitiesInRect(rect, temps);
	for (auto entity : temps)
	{
		if ( entity->GetLayer() == srcLayer && entity->IsObstacle(src))
			obstacles.push_back(entity);
	}
	temps.clear();

	return obstacles;
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

void Map::SetBackground(SpritePtr background)
{
	mBackGround = background;
	mBackGround->SetGlobalOrder(-1);
	mBackGround->SetDeferredDraw(true);
}

std::string Map::GetDestination() const
{
	return mDestinationName;
}

void Map::SetDestination(const std::string & name)
{
	mDestinationName = name;
}

Destination * Map::GetDestination()
{
	Debug::CheckAssertion(IsLoaded(), "Get destination, but the map is not loaded");
	std::shared_ptr<Destination> destination = nullptr;
	
	if (mDestinationName != "")
	{
		const auto& findEntity = GetEntities().FindEntity(mDestinationName);
		if (findEntity == nullptr || findEntity->GetEntityType() != EntityType::DESTIMATION)
		{
			Debug::Error("The Destination named'" + mDestinationName + "' is invalid");
			return nullptr;
		}
		destination = std::static_pointer_cast<Destination>(findEntity);
	}
	return destination.get();
}

/**
*	\brief 获取相机左上角坐标
*/
Point2 Map::GetCameraLeftTopPos() const
{
	if (mCamera == nullptr)
	{
		Debug::Warning("Get the camear pos without camera.");
		return Point2();
	}
	return mCamera->GetLeftTopPos();
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
	return TestCollisionWithEntities(rect, entity, layer);
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
*	\brief 测试entity与障碍物entity的碰撞检测
*	\param rect 测试entity的包围盒rect
*	\param entity 参与碰撞检测的entity
*	\return true 发生了碰撞检测
*/
bool Map::TestCollisionWithEntities(const Rect & rect, Entity & entity, int layer)
{
	std::vector<EntityPtr> entityNearby;
	mEntities->GetEntitiesInRect(rect, entityNearby);
	for (auto& checkEntity : entityNearby)
	{
		if (!checkEntity->IsSuspended() &&
			!checkEntity->IsBeRemoved() &&
			checkEntity->GetLayer() == layer &&
			checkEntity.get() != &entity &&
			checkEntity->IsObstacle(entity) &&
			checkEntity->TestCollisionWithRect(rect, Entity::BOUNDING_BOX_OBSTACLE))
			return true;
	}
	return false;
}

/**
*	\brief 检测实体与其他实体的碰撞
*
*	当其他实体可碰撞时且发生碰撞时，会触发notifyCollision
*/
void Map::CheckCollisionWithEntities(Entity & entity)
{
	if (IsSuspended())
		return;

	if (entity.IsBeRemoved() || !entity.IsEnable())
		return;

	Rect checkRect = entity.GetRectBounding();
	checkRect.Extend(8, 8);

	// 获取临近范围内的entity
	std::vector<EntityPtr> entityNearby;
	mEntities->GetEntitiesInRect(checkRect, entityNearby);
	for (auto& checkEntity : entityNearby)
	{
		if (!checkEntity->IsHaveCollision() ||
			checkEntity->IsBeRemoved() ||
			!checkEntity->IsEnable() ||
			checkEntity.get() == &entity)
		{
			continue;
		}
		checkEntity->CheckCollision(entity);
	}
}

/**
*	\brief 检测实体与其他实体的碰撞
*
*	当其他实体可碰撞时且发生碰撞时，会触发notifyCollision
*	与CheckCollisionWithEntities不同的是，这由entity主
*	动发起与其他entity的碰撞CheckCollision(*otherEntity);
*/
void Map::CheckCollisionFromEntities(Entity & entity)
{
	if (IsSuspended())
		return;

	if (entity.IsBeRemoved() || !entity.IsEnable())
		return;

	Rect checkRect = entity.GetRectBounding();
	checkRect.Extend(8, 8);
	std::vector<EntityPtr> entityNearby;
	mEntities->GetEntitiesInRect(checkRect, entityNearby);
	for (auto& checkEntity : entityNearby)
	{
		if (entity.IsBeRemoved())
			return;	// 可能因为碰撞而直接销毁

		if (!checkEntity->IsSuspended() &&
			!checkEntity->IsBeRemoved() &&
			checkEntity->IsEnable() && 
			checkEntity.get() != &entity ) 
		{
			entity.CheckCollision(*checkEntity);
		}
	}
}

/**
*	\brief 检测实体与其他实体的像素级碰撞
*
*	当其他实体可碰撞时且发生碰撞时，会触发notifyCollision
*	与CheckCollisionWithEntities不同的是，这由entity主
*	动发起与其他entity的碰撞CheckCollision(*otherEntity);
*/
void Map::CheckCollisionWithEntities(Entity & entity, Sprite & sprite)
{
	if(IsSuspended())
		return;

	if (entity.IsBeRemoved() || !entity.IsEnable())
		return;

	Rect checkRect = entity.GetRectBounding();
	// 获取临近范围内的entity
	std::vector<EntityPtr> entityNearby;
	mEntities->GetEntitiesInRect(checkRect, entityNearby);
	for (auto& checkEntity : entityNearby)
	{
		if (entity.IsBeRemoved())
			return;

		if(checkEntity->IsHaveCollision() &&
			!checkEntity->IsBeRemoved() && 
			!checkEntity->IsSuspended() &&
			checkEntity.get() != & entity)
		checkEntity->CheckSpriteCollision(entity, sprite);
	}
}

void Map::CheckCollisionFromEntities(Entity & entity, Sprite & sprite)
{
	// TODO
	return;
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
	// test
	mMapGenerate.DrawDebug();

	DrawBackground();
	mEntities->Draw();
	//DrawForeground();

	GetLuaContext().OnMapDraw(*this);
}

/**
*	\brief 绘制背景图
*/
void Map::DrawBackground()
{
	if (mBackGround)
		mBackGround->Draw();
}

/**
*	\brief 绘制前景图
*/
void Map::DrawForeground()
{
	if (mFrontGround)
		mFrontGround->Draw();
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
