#include "noAnimatedTilePattern.h"
#include "entity\tileset.h"

NoAnimatedTilePattern::NoAnimatedTilePattern(const TilePatternData & data):
	TilePattern(data)
{
}

void NoAnimatedTilePattern::Draw(const Point2 & pos, const Size & size, const Tileset & tileset) const
{
	auto& tilesetImg = tileset.GetTileImage();
	tilesetImg->SetTextureRect(Rect(GetPos(), GetSize()), true);
	tilesetImg->SetPos(pos);
	tilesetImg->SetSize(size);

	tilesetImg->Draw();
}
