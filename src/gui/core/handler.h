#pragma once

// gui core/8.8.2017 zy

#include"common\common.h"
#include"core\inputEvent.h"
#include"utils\typeSet.h"

#include<set>


namespace gui
{

class Widget;
class Dispatcher;

class GUIManager
{
public:
	GUIManager();
	~GUIManager();
	void HandleEvent(const InputEvent& event);
};

/***** ui事件类型和相关集合  **/
enum ui_event
{
	UI_EVENT_UNKNOW,
	UI_EVENT_ACTIVATE,
	UI_EVENT_DRAW,
	UI_EVENT_CLOSEWINDOW,

	EVENT_MOUSE_BUTTONDOWN,
	EVENT_MOUSE_BUTTONUP,
	EVENT_MOUSE_MOTION,

	UI_EVENT_MOUSE_ENTER,
	UI_EVENT_MOUSE_LEAVE,
	UI_EVENT_MOUSE_MOTION,
	UI_EVENT_MOUSE_LEFT_BUTTON_DOWN,
	UI_EVENT_MOUSE_RIGHT_BUTTON_DOWN,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_DOWN,
	UI_EVENT_MOUSE_LEFT_BUTTON_UP,
	UI_EVENT_MOUSE_RIGHT_BUTTON_UP,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_UP,

	UI_EVENT_KEY_DOWN,
	UI_EVENT_KEY_UP,

	EVENT_KEYBOARD_KEYDOWN,
	EVENT_KEYBOARD_KEYUP,

	EVENT_NOTIFY_REMOVE,
	EVENT_NOTIFY_MODIFIED,

	EVENT_REQUEST_PLACEMENT				

};

template<ui_event V>
using int_ = std::integral_constant<int, V>;

using setEvent = util::typeset< int_<UI_EVENT_ACTIVATE>,
								int_<UI_EVENT_DRAW>,
								int_<UI_EVENT_CLOSEWINDOW>,
								int_<UI_EVENT_MOUSE_LEAVE>> ;
	
using setEventMouse = util::typeset<int_<UI_EVENT_MOUSE_MOTION>,
									int_<UI_EVENT_MOUSE_LEFT_BUTTON_DOWN>,
									int_<UI_EVENT_MOUSE_RIGHT_BUTTON_DOWN>,
									int_<UI_EVENT_MOUSE_MIDDLE_BUTTON_DOWN >> ;

using setEventKeyboard = util::typeset<int_<UI_EVENT_KEY_DOWN>,int_<UI_EVENT_KEY_UP>>;

/**
*	\brief 将一个dispatcher链接到event handlder
*/
void ConnectDispatcher(Dispatcher* Dispatcher);

/**
*	\brief 将一个dispatcher从event handlder中解除链接
*/
void DisconnectDispatcher(Dispatcher* dispatcher);

}