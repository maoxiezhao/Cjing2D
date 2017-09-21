#include "noAnimatedTilePattern.h"
#include "game\sprite.h"
#include "entity\tileset.h"

NoAnimatedTilePattern::NoAnimatedTilePattern(const TilePatternData & data):
	TilePattern(data),
	mTileSprite(nullptr)
{
}

/**
*	\brief ���ص�ǰpattern��Ӧ���������
*/
Rect NoAnimatedTilePattern::GetTextureRect() const
{
	return Rect(GetPos(), GetSize());
}
