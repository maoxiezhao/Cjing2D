#pragma once

#include"entity\tilepattern.h"

/**
*	\brief ��֡�����Ļ�����ͼ��
*/
class NoAnimatedTilePattern : public TilePattern
{
public:
	NoAnimatedTilePattern(const TilePatternData& data);
	virtual Rect GetTextureRect()const;

private:
	SpritePtr mTileSprite;

};