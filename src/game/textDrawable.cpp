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
*	\brief �����ı�
*	
*	��Ϊ���ջ��ƻ�����font�����ƣ����Կ��ܻ��в�ͬ��TextDrawable
*	ӵ��ͬһ���������ڻ���ǰ��Ҫ���ú���������
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
*	\brief �����ı�
*	\param λ��

*	��Ϊ���ջ��ƻ�����font�����ƣ����Կ��ܻ��в�ͬ��TextDrawable
*	ӵ��ͬһ���������ڻ���ǰ��Ҫ���ú���������,����λ�������ɲ�
*	������
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
*	\brief ��������
*
*	����font�����˻����������ʽ��һ���fontAtlas�л�ȡ
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
*	\brief ���õ�ǰ�����ı�
*
*	���Ƶ��ı�������utf8����
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
*	\brief ��ȡ�ı����
*
*	�����ı���ȱ�����font�����¼������Ч�ʽϵ�
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
*	\brief ��ȡ�ı��߶�
*
*	�����ı���ȱ�����font�����¼������Ч�ʽϵ�
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
