#include"core\font.h"

namespace font {

Font::Font()
{
}

Font::Font(const string & font)
{
}

Font::~Font()
{
}

void Font::RenderText()
{
}

/**
*	\brief 渲染文字
*	\param size 子号
*	\param pos  位置
*	\param cols 列数
*	\param clipRect 裁剪矩形
*	\param renderText 渲染文字
*/
void Font::ReaderText(float size, const Point2 & pos, int cols, const Rect & clipRect, const string & renderText)
{
	// 字符解码

	// 字符绘制
}

/***** ***** *****  Glyph ***** ***** *****/

Font::Glyph* Font::FindGlyph()
{
	return nullptr;
}

const Font::Glyph* Font::FindGlyph() const
{
	return nullptr;
}

Font::Glyph::Glyph()
{
}

}