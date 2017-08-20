#pragma once

#include"common\common.h"
#include"font.h"

namespace font {

/**
*	\brief 字体配置项
*/
struct FontConfig
{
	FontConfig();

	string name;

	int   fontIndex;
	void* fontData;
	int   fontDataSize;
	Vec2i GlyphOffset;
	int   sizePixels;			

	bool  merageMode;
};

/**
*	\brief 字体集合
*/
class FontAtlas
{
public:
};

}
