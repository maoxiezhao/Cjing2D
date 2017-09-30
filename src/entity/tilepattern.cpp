#include "tilepattern.h"
#include "entity\tileset.h"
#include "entity\animatedTilePattern.h"

TilePattern::TilePattern(const TilePatternData & data):
	mPatternID(data.GetPatternID()),
	mPos(data.GetPos()),
	mSize(data.GetSize()),
	mGround(data.GetGround())
{
}

void TilePattern::Init()
{
	// do nothing
}

/**
*	\brief ÿ֡ˢ�º���
*/
void TilePattern::Update()
{
	AnimatedTilePattern::Update();
}

