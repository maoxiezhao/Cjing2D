#include "gui\widget\label.h"
#include "gui\lua\uiApi.h"
#include "gui\core\uiRender.h"

namespace gui {

Label::Label():
	StyledWidget(),
	mMultLine(false),
	mAlignFlag(FONT_ALIGN_LEFT | FONT_ALIGN_TOP),
	mFontSize(22),
	mFontColor(Color4B::WHITE),
	mLineHeight(24),
	mFontName(UIRender::GetSystemFontFace())
{
}

Label::Label(const std::string & text):
	StyledWidget(),
	mText(text),
	mMultLine(false),
	mAlignFlag(FONT_ALIGN_LEFT | FONT_ALIGN_TOP),
	mFontSize(22),
	mFontColor(Color4B::WHITE),
	mLineHeight(24),
	mFontName(UIRender::GetSystemFontFace())
{
}

std::string Label::GetText() const
{
	return mText;
}

void Label::SetAlign(int alignFlag)
{
	alignFlag = alignFlag;
}

int Label::GetAlign() const
{
	return mAlignFlag;
}

void Label::SetVerticalAlign(int alignFlag)
{
	mAlignFlag = mAlignFlag & 7 | alignFlag;
}

void Label::SetHorizontalAlign(int alignFlag)
{
	mAlignFlag = mAlignFlag & (15 < 3) | alignFlag;
}

std::string Label::GetFontFace() const
{
	return mFontName;
}

void Label::SetFontFace(const std::string & face)
{
	mFontName = face;
}

int Label::GetLineHeight() const
{
	return mLineHeight;
}

void Label::SetLineHeight(int lineHeight)
{
	mLineHeight = lineHeight;
}

void Label::SetFontColor(const Color4B & color)
{
	mFontColor = color;
}

Color4B Label::GetFontColor() const
{
	return mFontColor;
}

void Label::SetMultline(bool multed)
{
	mMultLine = multed;
}

int Label::GetMultline() const
{
	return mMultLine;
}

void Label::SetFontSize(int size)
{
	mFontSize = size;
}

int Label::GetFontSize() const
{
	return mFontSize;
}

void Label::SetText(const std::string & text)
{
	mText = text;
}

void Label::ImplDrawBackground(const Point2 & offset)
{
	TextRenderInfo info;
	info.text = GetText();
	info.fontName = mFontName;
	info.fontAlign = mAlignFlag;
	info.fontColor = mFontColor;
	info.fontRect = { GetPositon(), GetSize() };
	info.fontSize = mFontSize;
	info.multline = mMultLine;
	info.lineHeight = mLineHeight;

	UIRender::RenderLabel(info);
}

const string Label::GetLuaObjectName() const
{
	return ui_lua_lable_name;
}


}