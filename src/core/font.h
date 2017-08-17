#pragma once

#include"common\common.h"
#include"core\textureCache.h"
#include"game\sprite.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"

namespace font{

/**
*	\brief Font 字体基类
*
*	基于freetype实现字形渲染至纹理
*	以及简单提供文本布局功能，Font类维护一个字体纹理
*	字体纹理保存着当前使用最多的一系列字形，每次update
*	时，会删除使用最少的字，以便为新增加的字形提供空间
*/
class Font
{
public:
	Font();
	~Font();

	Font(const Font& other) = delete;
	Font& operator=(const Font& other) = delete;	

	void RenderText();
	void ReaderText(float size, const Point2& pos, int cols, const Rect& clipRect, const string& renderText);
	
private:
	/*** *** ***  Glyph *** *** ***/
	struct Glyph
	{
	public:
		Glyph();

	};

	Glyph& FindGlyph();
	const Glyph& FindGlyph()const;

public:


private:
	


};

}