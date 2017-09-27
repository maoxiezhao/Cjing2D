#include "textDrawable.h"
#include "lua\luaContext.h"

TextDrawable::TextDrawable():
	mFont(nullptr),
	mFontColor(Color4B::WHITE),
	mFontSize(0),
	mTextLineHeight(0),
	mTextLetterSpacing(0),
	mTextHorizontalAlign(font::TEXT_ALIGN_LEFT),
	mText("")
{
}

/**
*	\brief 绘制文本
*	
*	因为最终绘制还是由font来绘制，所以可能会有不同的TextDrawable
*	拥有同一个，所以在绘制前需要设置好所有属性
*/
void TextDrawable::Draw()
{
	if (mFont != nullptr)
	{
		mFont->SetFontColor(mFontColor);
		mFont->SetLineHeight(mTextLineHeight);
		mFont->SetLetterSpacing(mTextLetterSpacing);
		mFont->RenderText((float)mFontSize, mPos, mText, mTextHorizontalAlign);
	}
}


/**
*	\brief 绘制文本
*	\param 位置

*	因为最终绘制还是由font来绘制，所以可能会有不同的TextDrawable
*	拥有同一个，所以在绘制前需要设置好所有属性,其中位置属性由参
*	数传入
*/
void TextDrawable::Draw(const Point2 & pos)
{
	if (mFont != nullptr)
	{
		mFont->SetFontColor(mFontColor);
		mFont->SetLineHeight(mTextLineHeight);
		mFont->SetLetterSpacing(mTextLetterSpacing);
		mFont->RenderText((float)mFontSize, pos, mText, mTextHorizontalAlign);
	}
}

const string TextDrawable::GetLuaObjectName() const
{
	return LuaContext::module_font_name;
}

/**
*	\brief 设置字体
*
*	字体font决定了绘制字体的样式，一般从fontAtlas中获取
*/
void TextDrawable::SetFont(const font::FontPtr & font)
{
	if (font != mFont)
	{
		mFont = font;
		mFontSize = font->GetFontSize();
		mTextLineHeight = font->GetLineHeight();
		mTextLetterSpacing = font->GetLetterSpacing();
	}
}

const font::FontPtr & TextDrawable::getFont() const
{
	return mFont;
}

/**
*	\brief 设置当前绘制文本
*
*	绘制的文本必须是utf8编码
*/
void TextDrawable::SetText(const string & text)
{
	mText = text;
}

const string & TextDrawable::GetText() const
{
	return mText;
}

bool TextDrawable::IsTextEmpty() const
{
	return mText.empty();
}

void TextDrawable::SetTextColor(const Color4B & color)
{
	mFontColor = color;
}

const Color4B & TextDrawable::GetTextColor() const
{
	return mFontColor;
}

void TextDrawable::SetTextHorizontalAlign(unsigned int textHorizontalAlign)
{
	if ((textHorizontalAlign & font::TEXT_ALIGN_MASK) == 0)
	{
		Debug::Error("Invaild text horizontal alignment in textDrawable.");
	}
	mTextHorizontalAlign = textHorizontalAlign;
}

unsigned int TextDrawable::GetTextHorizontalAlign() const
{
	return mTextHorizontalAlign;
}

void TextDrawable::SetFontSize(int fontSise)
{
	mFontSize = fontSise;
}

int TextDrawable::GetFontSize() const
{
	return mFontSize;
}

void TextDrawable::SetLineHeight(int lineHeight)
{
	mTextLineHeight = lineHeight;
}

int TextDrawable::GetLineHeight() const
{
	return mTextLineHeight;
}

void TextDrawable::SetLetterSpacing(int letterSpacing)
{
	mTextLetterSpacing = letterSpacing;
}

int TextDrawable::GetLetterSpacing() const
{
	return mTextLetterSpacing;
}

/**
*	\brief 获取文本宽度
*
*	绘制文本宽度必须在font中重新计算过，效率较低
*/
int TextDrawable::GetWidth() const
{
	if (mFont == nullptr || IsTextEmpty())
	{
		return 0;
	}
	return mFont->GetTextWidth(mText);
}

/**
*	\brief 获取文本高度
*
*	绘制文本宽度必须在font中重新计算过，效率较低
*/
int TextDrawable::GetHeight() const
{
	if (mFont == nullptr || IsTextEmpty())
	{
		return 0;
	}
	return mFont->GetTextHeight(mText);
}


Size TextDrawable::GetSize() const
{
	if (mFont == nullptr || IsTextEmpty())
	{
		return Size();
	}
	int width = GetWidth();
	int height = GetHeight();
	return Size(width, height);
}
