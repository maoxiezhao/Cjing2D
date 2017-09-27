#pragma once

#include"core\fontAtlas.h"
#include"game\drawable.h"

/**
*	\brief 文字绘制体
*
*	文字绘制体封装了底层font的接口，并派生于drawabel,可以以LuaObject的形式
*	参与到脚本行为中。
*	文字绘制体从功能上说类似于gui::label,不同的是提供了一种更为轻便或者说代
*	替方案(不参与gui的事件分发)，未来应更应该使用gui:label。
*/
class TextDrawable : public Drawable
{
public:
	TextDrawable();
	
	virtual void Draw();
	virtual void Draw(const Point2& pos);
	virtual const string GetLuaObjectName()const;

	/***** ***** ****** setter/gettter ***** ***** ******/
	void SetFont(const font::FontPtr& font);
	const font::FontPtr& getFont()const;

	void SetText(const string& text);
	const string& GetText()const;
	bool IsTextEmpty()const;

	void SetTextColor(const Color4B& color);
	const Color4B& GetTextColor()const;

	void SetTextHorizontalAlign(unsigned int textHorizontalAlign);
	unsigned int GetTextHorizontalAlign()const;

	void SetFontSize(int fontSise);
	int GetFontSize()const;

	void SetLineHeight(int lineHeight);
	int GetLineHeight()const;

	void SetLetterSpacing(int letterSpacing);
	int GetLetterSpacing()const;

	int GetWidth()const;
	int GetHeight()const;
	Size GetSize()const;

private:

	font::FontPtr mFont;

	Color4B mFontColor;
	int  mFontSize;
	unsigned int mTextHorizontalAlign;
	std::string  mText;
	int mTextLineHeight;
	int mTextLetterSpacing;
};

using TextDrawablePtr = std::shared_ptr<TextDrawable>;