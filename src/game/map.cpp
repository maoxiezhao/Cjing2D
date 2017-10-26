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
*	\brief ���ص�ͼ
*
*	��ͼ���ݰ���.dat���ݺ�.lua�ű���.datΪ��ͼ����entity����
*	��loadʱ����أ���.lua�ű�����startʱ��luaContext�м���
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
	// ���ص�ͼ�ɹ������ʼ����ͼ����
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
*	\brief ���ʵ��������ʵ�����ײ
*
*	������ʵ�����ײʱ�ҷ�����ײʱ���ᴥ��notifyCollision
*/
void Map::CheckCollisionWithEntities(Entity & entity)
{
	Rect checkRect = entity.GetRectBounding();
	checkRect.Extend(8, 8);

	// ��ȡ�ٽ���Χ�ڵ�entity
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
	DrawBackground();
	mEntities->Draw();
	DrawForeground();

	GetLuaContext().OnMapDraw(*this);
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
