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
*	\brief ÿ֡ˢ�º���
*/
void TilePattern::Update()
{
	AnimatedTilePattern::Update();
}

/**
*	\brief ���ƹ���
*
*	���ݵ�ǰpattern״̬��tileset imgage����
*/
void TilePattern::Draw(const Point2& pos, const Size& size, const Tileset& tileset)const
{
}
