#include "tilepattern.h"
#include "entity\tileset.h"

TilePattern::TilePattern(const TilePatternData & data):
	mPatternID(data.GetPatternID()),
	mPos(data.GetPos()),
	mSize(data.GetSize())
{
}

/**
*	\brief ���ƹ���
*
*	���ݵ�ǰpattern״̬��tileset imgage����
*/
void TilePattern::Draw(const Point2& pos, const Size& size, const Tileset& tileset)const
{
	auto& tilesetImg = tileset.GetTileImage();
	tilesetImg->SetTextureRect(Rect(mPos, mSize), true);
	tilesetImg->SetPos(pos);
	tilesetImg->SetSize(size);

	tilesetImg->Draw();
}
