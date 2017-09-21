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
