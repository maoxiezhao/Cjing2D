#include "tilepattern.h"
#include "entity\tileset.h"
#include "entity\animatedTilePattern.h"

TilePattern::TilePattern(const TilePatternData & data):
	mPatternID(data.GetPatternID()),
	mPos(data.GetPos()),
	mSize(data.GetSize())
{
}

void TilePattern::Init()
{
	// do nothing
}

/**
*	\brief Ã¿Ö¡Ë¢ÐÂº¯Êý
*/
void TilePattern::Update()
{
	AnimatedTilePattern::Update();
}

