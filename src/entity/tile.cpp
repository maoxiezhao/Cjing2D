#include "tile.h"
#include "game\map.h"

Tile::Tile(const TileInfo& tileInfo):
	Entity("", tileInfo.mPos, tileInfo.mSize, tileInfo.mLayer),
	mPatternID(tileInfo.mPatternID),
	mPattern(*tileInfo.mPattern)
{
}

void Tile::Update()
{
}

void Tile::Draw()
{
	mPattern.Draw(GetPos(), GetSize(), GetMap().GetTileset());
}
