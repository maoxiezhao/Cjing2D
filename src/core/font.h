#pragma once

#include"common\common.h"
#include"core\textureCache.h"
#include"game\sprite.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"
#include"core\renderer.h"

#include"ft2build.h"
#include"freetype.h"
#define FT_FREETYPE_H

namespace font{

struct FontConfig;

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
	enum
	{
		FONT_CODE_LATIN_START = 0x0021,
		FONT_CODE_LATIN_END   = 0x00A6,
		FONT_CODE_CHINESE_START = 0x4E00,
		FONT_CODE_CHINESE_END  = 0x9FA5,
	};

	Font();
	Font(const string& font);
	~Font();

	Font(const Font& other) = delete;
	Font& operator=(const Font& other) = delete;	

	void UnLoad();
	void Flush();
	void LoadFont();
	void BuildLookupTable();

	void RenderText();
	void RenderText(float size, const Point2& pos, const Rect& clipRect, const string& renderText);
	void RenderText(float size, const Point2 & pos, float wrapWidth, const Rect & clipRect, const char* textBegin, const char* textEnd);

	/**** **** ****  setter/gettter **** **** ****/
	bool IsLoaded()const
	{
		return mIsLoaded;
	}
	bool IsDirty()const
	{
		return mIsDirty;
	}

private:
	/**** **** ****  Glyph **** **** ****/
	/*
	*	\brief 字形结构，描述了字形在纹理中的位置
	*	以及绘制的相关信息
	*/
	struct Glyph
	{		
		wchar charCode;
		GLenum chanel;
		float advance;		// 字的起点到下一个字的起点
		float u0, v0, u1, v1;
	};

	float mFontSize;
	float mScale;
	float mAdvance;
	float mAscent;			// baseline之上至字符最高处的距离
	float mDescent;			// baseline之下至字符最低处的距离

	std::map<wchar, Glyph> mGlyphs;	// 存储所有的glyph

	std::map<Glyph, uint32_t> mTimesGlyphs;	// LRU的方式维护该表，表中存储有调用次数

	string mFontName;
	FontConfig* mConfig;
	FT_Library ft;
	FT_Face face;

public:
	Glyph* FindGlyph(wchar code);
	const Glyph* FindGlyph(wchar code)const;
	void InsertGlyph(const Glyph& glyph);

	void SetFontColor(const Color4B& color);
	const Color4B GetFontColor()const;

	void SetDynamicLoadTexture(bool isDynamicLoad , const Size& size);
	bool IsDynamicLoadTexture()const;
private:
	/**** **** ****  Render **** **** ****/
	void InitRender();
	void CalcTextureRect(int width, int height);
	void SetCharSize(const Size& size);
	void SetCharTexs(float u0, float v0, float u1, float v1);
	void SetCharScale(float scale);

	void Render(const Point2 & pos, const Size& size, float rotate);
	void Render(Renderer& renderer, const Matrix4& transform);

	Quad mQuad;					
	QuadCommand mQuadCommand;
	TexturePtr mTexture;	
	GLProgramStatePtr mProgramState;
	BlendFunc mBlendFunc;
	Matrix4 mModelView;

	bool mIsDynamicLoad;			// 是否全部加载
	Size mFontTextureSize;
	bool mIsDirty;
	bool mIsLoaded;
};

using FontPtr = std::shared_ptr<Font>;
using FontConstPtr = std::shared_ptr<const Font>;

}