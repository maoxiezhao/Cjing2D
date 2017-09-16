#include "tilepattern.h"
#include "entity\tileset.h"

TilePattern::TilePattern(const TilePatternData & data):
	mPatternID(data.GetPatternID()),
	mPos(data.GetPos()),
	mSize(data.GetSize())
{
}

/**
*	\brief 绘制过程
*
*	根据当前pattern状态对tileset imgage绘制
*/
void TilePattern::Draw(const Point2& pos, const Size& size, const Tileset& tileset)const
{
	auto& tilesetImg = tileset.GetTileImage();
	tilesetImg->SetTextureRect(Rect(mPos, mSize), true);
	tilesetImg->SetPos(pos);
	tilesetImg->SetSize(size);

	tilesetImg->Draw();
}
