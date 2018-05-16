#pragma once

#include"gui\widget\styledwidget.h"

namespace gui {

enum FontAlign {
	// Horizontal align
	FONT_ALIGN_LEFT		= 1 << 0,	// Ĭ���ı������
	FONT_ALIGN_CENTER   = 1 << 1,	// �ı����ж���
	FONT_ALIGN_RIGHT    = 1 << 2,	// �ı��Ҷ���
	// Vertical align
	FONT_ALIGN_TOP      = 1 << 3,	// �ı�ˮƽ��������
	FONT_ALIGN_MIDDLE   = 1 << 4,	// �ı�ˮƽ���ж���
	FONT_ALIGN_BOTTOM   = 1 << 5,	// �ı�ˮƽ�ײ�����
	FONT_ALIGN_BASELINE = 1 << 6,   //Ĭ�ϣ������ı���baseline
};

struct TextRenderInfo {
	bool multline;

	float fontSize;
	float lineHeight;
	Color4B fontColor;
	int fontAlign;

	std::string fontName;
	std::string text;
	Rect fontRect;
};

/**
*	\brief widget Label
*
*	Ŀǰ��Ȼʹ�õ���Nanovg��Ⱦ�⣬δ������һ��ֱ��ʹ��ϵͳ��fontSystem
*	�������޸�Nanovg����Ⱦ����
*/

class Label : public StyledWidget
{
public:
	Label();
	Label(const std::string& text);

	void SetText(const std::string& text);
	std::string GetText()const;
	void SetAlign(int alignFlag);
	int GetAlign()const;
	void SetVerticalAlign(int alignFlag);
	void SetHorizontalAlign(int alignFlag);
	std::string GetFontFace()const;
	void SetFontFace(const std::string& face);
	int GetLineHeight()const;
	void SetLineHeight(int lineHeight);
	void SetFontColor(const Color4B& color);
	Color4B GetFontColor()const;
	void SetMultline(bool multed);
	int GetMultline()const;
	void SetFontSize(int size);
	int GetFontSize()const;

	virtual void ImplDrawBackground(const Point2& offset);

	virtual WIDGET_TYPE GetWidgetType()const
	{
		return WIDGET_TYPE::WIDGET_LABEL;
	}

	virtual const string GetLuaObjectName()const;
private:
	std::string mText;
	std::string mFontName;

	bool mMultLine;
	int mAlignFlag;
	int mFontSize;
	int mLineHeight;

	Color4B mFontColor;
};
}