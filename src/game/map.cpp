#include "map.h"
#include "core\inputEvent.h"
#include "game\game.h"
#include "lua\luaContext.h"

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

void Map::Update()
{
	CheckSuspended();

	DrawBackground();
	
	mEntities->Update();
	GetLuaContext().OnMapUpdate();

	DrawForeground();
}

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

void Map::Draw()
{
	if (!IsLoaded())
	{
		return;
	}

	mEntities->Draw();
	GetLuaContext().OnMapDraw();
}