#pragma once

#include"gui\core\dispatcher.h"


/**
*	\brief distributor 
*	���ڴ���������ض���widget�ĸ߲��¼�,�����������¼��������ͣ�¼���
*	��Ҫ�������������Ͻ��¼��ַ���������widget
*/
namespace gui
{	

class Widget;

/**
*	\brief ����ƶ��¼�
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
*	\brief ������¼�
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
	Widget* mFocus;		// �����¼���widget
	Widget* mLastClickWidget;
	uint32_t mLastClickTime;
	uint32_t mLastDownTime;
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