#include "noAnimatedTilePattern.h"
#include "game\sprite.h"
#include "entity\tileset.h"

NoAnimatedTilePattern::NoAnimatedTilePattern(const TilePatternData & data):
	TilePattern(data),
	mTileSprite(nullptr)
{
}

/**
*	\brief 返回当前pattern对应的纹理矩形
*/
Rect NoAnimatedTilePattern::GetTextureRect() const
{
	return Rect(GetPos(), GetSize());
}
