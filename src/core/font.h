#pragma once

#include"common\common.h"
#include"core\textureCache.h"
#include"core\renderer.h"
#include"game\sprite.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"

#include"ft2build.h"
#include"freetype.h"
#define FT_FREETYPE_H

namespace font{

struct FontConfig;
struct FontCodeRange;

static const unsigned int TEXT_ALIGN_SHIFT = 0;
static const unsigned int TEXT_ALIGN_LEFT = 1 << TEXT_ALIGN_SHIFT;
static const unsigned int TEXT_ALIGN_CENTER = 2 << TEXT_ALIGN_SHIFT;
static const unsigned int TEXT_ALIGN_RIGHT = 4 << TEXT_ALIGN_SHIFT;
static const unsigned int TEXT_ALIGN_MASK = 7 << TEXT_ALIGN_SHIFT;

/**
*	\brief Font �������
*
*	����freetypeʵ��������Ⱦ�������Լ����ṩ�ı�����
*	���ܣ�Font��ά��һ���������������������ŵ�ǰʹ��
*	����һϵ�����Σ�ÿ��updateʱ����ɾ��ʹ�����ٵ���
*	�Ա�Ϊ�����ӵ������ṩ�ռ�.Font����Ⱦ����״̬ģʽ
*
*	���ڽ���Ӷ�̬�汾
*
*	TODO: �޸ļ��ط�ʽ 18/5/18
*/
class Font
{
public:
	enum
	{
		FONT_CODE_LATIN_START = 0x0021,
		FONT_CODE_LATIN_END = 0x00A6,
		FONT_CODE_CHINESE_START = 0x4E00,
		FONT_CODE_CHINESE_END = 0x9FA5,
	};

	/**
	*	\brief ���ص��ַ���Χ����Ҫ���ڷǶ�̬����
	*/
	struct FontCodeRange
	{
		unsigned int codeBegin;
		unsigned int codeEnd;

		FontCodeRange(unsigned int begin, unsigned int end) :codeBegin(begin), codeEnd(end) {}
	};

public:	

	Font();
	Font(const string& font);
	~Font();

	Font(const Font& other) = delete;
	Font& operator=(const Font& other) = delete;	

	void UnLoad();
	void Flush();
	void LoadFont(const string& fontData,const std::vector<FontCodeRange>& codeRanges);
	void BuildLookupTable();

	void RenderText(float size, const Point2 & pos, const string& renderText, unsigned int textAlign = TEXT_ALIGN_LEFT);		/** ���˵Ļ��ƽӿ� */
	void RenderText(float size, const Point2 & pos, const Rect & clipRect, float wrapWidth, const string& renderText, unsigned int textAlign = TEXT_ALIGN_LEFT);
	void RenderText(float size, const Point2& pos, const Rect& clipRect, float wrapWidth, const char* textBegin, const char* textEnd = nullptr, unsigned int textAlign = TEXT_ALIGN_LEFT);

	bool IsLoaded()const;
	bool IsDirty()const;

	int GetTextWidth(const string& text)const;
	int GetTextHeight(const string& text)const;
private:

	/**** **** ****  Glyph **** **** ****/
	/*
	*	\brief ���νṹ�������������������е�λ��
	*	�Լ����Ƶ������Ϣ
	*/
	struct Glyph
	{		
		wchar charCode;
		GLenum chanel;
		int    advance;		// �ֵ���㵽��һ���ֵ����
		Size   bearing;
		Size   size;
		float u0, v0, u1, v1;

		Glyph() :charCode(0), chanel(0), advance(0), bearing(), size(), u0(0), v0(0), u1(0), v1(0) {}
	};

	int   mFontSize;
	float mScale;
	float mAdvance;
	int   mAscent;			// baseline֮�����ַ���ߴ��ľ���
	int   mDescent;			// baseline֮�����ַ���ʹ��ľ���
	int   mLetterSpacing;	// �ּ��
	int   mLineHeight;		// �м��

	Glyph mFallbackChar;	// ����Ҫ��Ⱦ���ֲ�����ʱ��ʹ�ø��ַ�

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
	void SetLineHeight(int lineHeight);
	int GetLineHeight()const;

	void SetDynamicLoadTexture(bool isDynamicLoad , const Size& size);
	bool IsDynamicLoadTexture()const;
private:
	/**** **** ****  Render **** **** ****/
	const char* GetWordWraoPosition(const char * textBegin, const char * textEnd, float scale, float wrapWidth);
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

	bool mIsDynamicLoad;			// �Ƿ�ȫ������
	Size mFontTextureSize;
	bool mIsDirty;
	bool mIsLoaded;
};

using FontPtr = std::shared_ptr<Font>;
using FontConstPtr = std::shared_ptr<const Font>;

namespace implementation
{
	int ParsedUtf8(unsigned int& outChar, const char* textBegin, const char* textEnd);
}

}