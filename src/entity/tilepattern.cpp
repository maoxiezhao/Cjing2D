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
*	\brief 每帧刷新函数
*/
void TilePattern::Update()
{
	AnimatedTilePattern::Update();
}

/**
*	\brief 绘制过程
*
*	根据当前pattern状态对tileset imgage绘制
*/
void TilePattern::Draw(const Point2& pos, const Size& size, const Tileset& tileset)const
{
}
