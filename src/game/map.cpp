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
*	\brief ���ص�ͼ
*
*	��ͼ���ݰ���.dat���ݺ�.lua�ű���.datΪ��ͼ����entity����
*	��loadʱ����أ���.lua�ű�����startʱ��luaContext�м���
*/
void Map::Load(Game * game)
{
	// ���ص�ͼ�ɹ������ʼ����ͼ����
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

	// ���ط�������
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

	// ������������
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
*	\brief ��ʼ��ͼ,ִ��lua��runMap����
*/
void Map::Start()
{
	mIsStarted = true;
	mEntities->NotifyMapStarted();
	GetLuaContext().RunMap(*this);
}

/**
*	\brief �뿪��ͼ
*/
void Map::Leave()
{
	mIsStarted = false;
	mEntities->NotifyMapStoped();
	GetLuaContext().LeaveMap(*this);
}

/**
*	\brief ��ͼˢ��
*/
void Map::Update()
{
	CheckSuspended();

	// ����entity
	mEntities->Update();

	// ����tilePattern
	TilePattern::Update();

	GetLuaContext().OnMapUpdate(*this);
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

/**
*	\brief �Ƿ��ǺϷ���layer
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
*	\brief ��ȡ��ΧRect�ڵ������ϰ���
*/
std::vector<EntityPtr> Map::GetObstacles(const Rect & rect, Entity & src) const
{
	std::vector<EntityPtr> obstacles;	
	int srcLayer = src.GetLayer();

	// û��Ҫ������vector
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
*	\brief ���õ�ǰtileset
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
*	\brief ��ȡ������Ͻ�����
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
*	\brief ����entity���ϰ������ײ���
*	\param rect ����entity�İ�Χ��rect
*	\param entity ������ײ����entity
*	\return true ��������ײ���
*/
bool Map::TestCollisionWithObstacle(const Rect & rect, Entity & entity)
{
	// ��map��ǰlayerd��ground
	int xBegin = rect.x;
	int xEnd = xBegin + rect.width - 1;
	int yBegin = rect.y;
	int yEnd = yBegin + rect.height - 1;
	int layer = entity.GetLayer();

	// ������4���߽缴��
	for (int x = xBegin; x <= xEnd; x += 8)
	{
		if (TestCollisionWithGround(layer, x, yBegin, entity) ||
			TestCollisionWithGround(layer, x, yEnd, entity) ||
			TestCollisionWithGround(layer, x + 7, yBegin, entity) ||
			TestCollisionWithGround(layer, x + 7, yEnd, entity)){
			return true;
		}
	}

	// ������ÿ8������Ϊ���ĵ��ground�Ƿ����ϰ���
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
*	\brief ����entity���ϰ���entity����ײ���
*	\param rect ����entity�İ�Χ��rect
*	\param entity ������ײ����entity
*	\return true ��������ײ���
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
*	\brief ���ʵ��������ʵ�����ײ
*
*	������ʵ�����ײʱ�ҷ�����ײʱ���ᴥ��notifyCollision
*/
void Map::CheckCollisionWithEntities(Entity & entity)
{
	if (IsSuspended())
		return;

	if (entity.IsBeRemoved() || !entity.IsEnable())
		return;

	Rect checkRect = entity.GetRectBounding();
	checkRect.Extend(8, 8);

	// ��ȡ�ٽ���Χ�ڵ�entity
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
*	\brief ���ʵ��������ʵ�����ײ
*
*	������ʵ�����ײʱ�ҷ�����ײʱ���ᴥ��notifyCollision
*	��CheckCollisionWithEntities��ͬ���ǣ�����entity��
*	������������entity����ײCheckCollision(*otherEntity);
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
			return;	// ������Ϊ��ײ��ֱ������

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
*	\brief ���ʵ��������ʵ������ؼ���ײ
*
*	������ʵ�����ײʱ�ҷ�����ײʱ���ᴥ��notifyCollision
*	��CheckCollisionWithEntities��ͬ���ǣ�����entity��
*	������������entity����ײCheckCollision(*otherEntity);
*/
void Map::CheckCollisionWithEntities(Entity & entity, Sprite & sprite)
{
	if(IsSuspended())
		return;

	if (entity.IsBeRemoved() || !entity.IsEnable())
		return;

	Rect checkRect = entity.GetRectBounding();
	// ��ȡ�ٽ���Χ�ڵ�entity
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
*	\brief ���Ƶ�ͼ
*
*	���ƹ������ΰ�������ͼ��entities,ǰ��ͼ,luaContent
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
*	\brief ���Ʊ���ͼ
*/
void Map::DrawBackground()
{
	if (mBackGround)
		mBackGround->Draw();
}

/**
*	\brief ����ǰ��ͼ
*/
void Map::DrawForeground()
{
	if (mFrontGround)
		mFrontGround->Draw();
}

/**
*	\brief �ڵ�ͼ�ϻ���drawable
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
*	\brief �ڵ�ͼ�ϻ���drawable
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
