#pragma once

#include"common\common.h"
#include"core\textureCache.h"
#include"game\sprite.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"
#include"core\renderer.h"

namespace font{

/**
*	\brief Font 字体基类
*
*	基于freetype实现字形渲染至纹理以及简单提供文本布局
*	功能，Font类维护一个字体纹理字体纹理保存着当前使用
*	最多的一系列字形，每次update时，会删除使用最少的字
*	以便为新增加的字形提供空间.Font的渲染采用状态模式
*
*	未来会使用Pangocario作为文字引擎
*/
class Font
{
public:
	Font();
	Font(const string& font);
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

	Glyph* FindGlyph();
	const Glyph* FindGlyph()const;

public:


private:
	void Render(Renderer& renderer, const Matrix4& transform);

	Quad mQuad;					
	QuadCommand mQuadCommmand; 
	TexturePtr mTexture;	

};

using FontPtr = std::shared_ptr<Font>;
using FontConstPtr = std::shared_ptr<const Font>;

}