#pragma once

#include"entity\tilepattern.h"

/**
*	\brief ��֡�����Ļ�����ͼ��
*/
class NoAnimatedTilePattern : public TilePattern
{
public:
	NoAnimatedTilePattern(const TilePatternData& data);
	virtual void Draw(const Point2& pos, const Size& size, const Tileset& tileset)const;
};