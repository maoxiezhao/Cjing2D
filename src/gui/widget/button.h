#pragma once

#include"gui\widget\styledwidget.h"

namespace gui
{
/**
*	\brief widget button
*
*	基本的Button类，可以设置3个图片或者3个颜色来定义按钮样式，如果设置为
*	使用系统样式则直接绘制，否则绘制三个sprite
*
*	TODO 考虑移除，毕竟可以在Lua中用Frame实现，但是还是先暂时留下该对象
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

	State mState;	// 当前按钮状态

	/** 绘制 */
	virtual void ImplDrawBackground(const Point2& offset);
	virtual void ImplDrawForeground(const Point2& offset);

	// test sprite
	SpritePtr mTestSprite;

	/** 信号回调函数 */
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