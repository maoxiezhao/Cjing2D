#pragma once

#include"entity\tilepattern.h"

/**
*	\brief 非帧动画的基本地图块
*/
class NoAnimatedTilePattern : public TilePattern
{
public:
	NoAnimatedTilePattern(const TilePatternData& data);
	virtual Rect GetTextureRect()const;

private:
	SpritePtr mTileSprite;

};