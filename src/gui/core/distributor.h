#pragma once

#include"gui\core\dispatcher.h"


/**
*	\brief distributor 
*	用于处理处理产生特定于widget的高层事件,例如鼠标进入事件，鼠标悬停事件等
*	主要用于容器主体上将事件分发给各个子widget
*/
namespace gui
{	

class Widget;

/**
*	\brief 鼠标移动事件
*/
class MouseMotion 
{
public:
	MouseMotion(Widget& widget, Dispatcher::queue_position position);
	~MouseMotion();

	void MouseCaptrue(bool captrued = true);

protected:
	void MouseEnter(Widget* widget);
	void MouseLeave();
	void MouseHover(Widget* widget, const Point2& coords);

	Widget& mOwner;
	Widget* mMouseFocus;

	void SignalHandleMouseMotion(const ui_event event, bool&handle, const Point2& coords);

	bool mMouseCaptured;
	bool mSignalHandelMouseMotionEntered;

};

template<ui_event buttonDown,
		 ui_event buttonUp,
		 ui_event buttonClick,
		 ui_event buttonDoubleClick>
struct mouseButtonEventWrapper
{
	static const ui_event buttonDownEvent = buttonDown;
	static const ui_event buttonUpEvent = buttonUp;
	static const ui_event buttonClickEvent = buttonClick;
	static const ui_event buttonDoubleClickEvent = buttonDoubleClick;
};

/**
*	\brief 鼠标点击事件
*/
template<typename T>
class MouseButton : public virtual MouseMotion
{
public:
	MouseButton(Widget& widget, Dispatcher::queue_position position);

private:
	bool mSignalHandlerButtonDownEntered;
	void mSignalHandlerButtonDown(const ui_event event, bool&handle, const Point2& coords);
	
	bool mSignalHandlerButtonUpEntered;
	void mSignalHandlerButtonUp(const ui_event event, bool&handle, const Point2& coords);

	void MouseButtonClick(Widget* widget);

	bool mIsDown;
	Widget* mFocus;		// 捕获事件的widget
	Widget* mLastClickWidget;
	uint32_t mLastClickTime;
};

using MouseButtonLeft = MouseButton<
	mouseButtonEventWrapper<
	UI_EVENT_MOUSE_LEFT_BUTTON_DOWN,
	UI_EVENT_MOUSE_LEFT_BUTTON_UP,
	UI_EVENT_MOUSE_LEFT_BUTTON_CLICK,
	UI_EVENT_MOUSE_LEFT_BUTTON_DOUBLE_CLICK>
>;

using MouseButtonMiddle = MouseButton<
	mouseButtonEventWrapper<
	UI_EVENT_MOUSE_MIDDLE_BUTTON_DOWN,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_UP,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_CLICK,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_DOUBLE_CLICK>
>;

using MouseButtonRight = MouseButton<
	mouseButtonEventWrapper<
	UI_EVENT_MOUSE_RIGHT_BUTTON_DOWN,
	UI_EVENT_MOUSE_RIGHT_BUTTON_UP,
	UI_EVENT_MOUSE_RIGHT_BUTTON_CLICK,
	UI_EVENT_MOUSE_RIGHT_BUTTON_DOUBLE_CLICK>
>;

class Distributor : public MouseButtonLeft
{
public:
	Distributor(Widget& widget, Dispatcher::queue_position position);
	~Distributor();

	void AddToKeyBoardFocusChain(Widget* widget);
	void RemoveKeyBoardFocusChain(Widget* widget);

private:
	std::vector<Widget*> mKeyboradFocusChain;

	void SignalHandlerKeyboradDown(const InputEvent::KeyboardKey key,const string & unicode);

	Widget* mKeyboardFocus;
};

}