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
	void RenderText(float size, const Point2 & pos, const Rect & clipRect, float wrapWidth, const string& renderText);
	void RenderText(float size, const Point2& pos, const Rect& clipRect, float wrapWidth, const char* textBegin, const char* textEnd = nullptr);

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
	/**
	*	\brief 加载的字符范围，主要用于非动态加载
	*/
	struct FontCodeRange
	{
		unsigned int codeBegin;
		unsigned int codeEnd;

		FontCodeRange(unsigned int begin, unsigned int end) :codeBegin(begin), codeEnd(end) {}
	};

	std::vector<FontCodeRange> mFontCodeRanges;

	/**** **** ****  Glyph **** **** ****/
	/*
	*	\brief 字形结构，描述了字形在纹理中的位置
	*	以及绘制的相关信息
	*/
	struct Glyph
	{		
		wchar charCode;
		GLenum chanel;
		int    advance;		// 字的起点到下一个字的起点
		Size   bearing;
		Size   size;
		float u0, v0, u1, v1;

		Glyph() :charCode(0), chanel(0), advance(0), bearing(), size(), u0(0), v0(0), u1(0), v1(0) {}
	};

	int   mFontSize;
	float mScale;
	float mAdvance;
	int   mAscent;			// baseline之上至字符最高处的距离
	int   mDescent;			// baseline之下至字符最低处的距离
	int   mLetterSpacing;	// 字间距

	Glyph mFallbackChar;	// 当需要渲染的字不存在时，使用该字符

	std::map<wchar, Glyph> mGlyphs;

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
	void SetFontSize(int size);
	int GetFontSize()const;
	void SetLetterSpacing(int spacing);
	int GetLetterSpacing()const;

	void SetDynamicLoadTexture(bool isDynamicLoad , const Size& size);
	bool IsDynamicLoadTexture()const;
private:
	/**** **** ****  Render **** **** ****/
	void InitRender();
	void CalcTextureRect(int width, int height);
	void SetCharSize(const Size& size);
	void SetCharTexs(float u0, float v0, float u1, float v1);
	void SetCharScale(float scale);
	void SetCharChanel(float chanel);

	void Render(const Point2 & pos, const Size& size, float rotate);
	void Render(Renderer& renderer, const Matrix4& transform);

	Quad mQuad;					
	QuadCommand mQuadCommand;
	GLProgramStatePtr mProgramState;
	TexturePtr mTexture;	
	BlendFunc mBlendFunc;
	Matrix4 mModelView;

	bool mIsDynamicLoad;			// 是否全部加载
	Size mFontTextureSize;
	bool mIsDirty;
	bool mIsLoaded;
};

using FontPtr = std::shared_ptr<Font>;
using FontConstPtr = std::shared_ptr<const Font>;

namespace implementation
{
	int ParsedUtf8(unsigned int& outChar, const char* textBegin, const char* textEnd);

	const char* GetWordWraoPosition(const char * textBegin, const char * textEnd,float scale, float wrapWidth);
}

}