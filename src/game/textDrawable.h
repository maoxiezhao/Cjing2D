#pragma once

#include"core\fontAtlas.h"
#include"game\drawable.h"

/**
*	\brief ���ֻ�����
*
*	���ֻ������װ�˵ײ�font�Ľӿڣ���������drawabel,������LuaObject����ʽ
*	���뵽�ű���Ϊ�С�
*	���ֻ�����ӹ�����˵������gui::label,��ͬ�����ṩ��һ�ָ�Ϊ������˵��
*	�淽��(������gui���¼��ַ�)��δ��Ӧ��Ӧ��ʹ��gui:label��
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