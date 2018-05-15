#pragma once

#include"gui\widget\styledwidget.h"

namespace gui
{
/**
*	\brief widget button
*
*	������Button�࣬��������3��ͼƬ����3����ɫ�����尴ť��ʽ���������Ϊ
*	ʹ��ϵͳ��ʽ��ֱ�ӻ��ƣ������������sprite
*
*	TODO �����Ƴ����Ͼ�������Lua����Frameʵ�֣����ǻ�������ʱ���¸ö���
*/
class Button : public StyledWidget
{
public:
	Button();

	virtual void SetActivite(const bool activite)const;
	virtual bool GetActivite()const;
	virtual unsigned int GetState()const;

	virtual WIDGET_TYPE GetWidgetType()const
	{
		return WIDGET_TYPE::WIDGET_BUTTON;
	}

	/** style test */
	bool IsUseSystemStyle()const;
	Color4B GetCurColor()const;

	virtual const string GetLuaObjectName()const;
private:
	enum State
	{
		ENABLED,
		DISABLED,
		FOCUSED,
		PRESSED,
		NUMSTATE,
	};

	void SetState(const State state)
	{
		mState = state;
	}

	State mState;	// ��ǰ��ť״̬

	/** ���� */
	virtual void ImplDrawBackground(const Point2& offset);
	virtual void ImplDrawForeground(const Point2& offset);

	// test sprite
	SpritePtr mTestSprite;

	/** �źŻص����� */
	void SignalHandlerMouseEnter(const  ui_event event, bool&handle);
	void SignalHandlerMouseLeave(const  ui_event event, bool&handle);
	void SignalHandlerMouseLeftButtonDown(const  ui_event event, bool&handle);
	void SignalHandlerMouseLeftButtonUp(const  ui_event event, bool&handle);
	void SignalHandlerMouseLeftButtonClick(const  ui_event event, bool&handle);

	// test
	void SignalHandlerMouseLeftButtonDoubleClick(const  ui_event event, bool&handle);

	bool mUseSystemStyle;

	Color4B mStateColor[NUMSTATE];
	std::map<int, SpritePtr> mStateSprite;
};
}