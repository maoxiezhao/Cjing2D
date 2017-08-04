#pragma once

#include"common\common.h"

namespace gui
{

class Widget;
class Dispatcher;

enum ui_event
{
	UI_EVENT_UNKNOW,
	UI_EVENT_MOUSE_MOTION,
	UI_EVENT_MOUSE_LEFT_BUTTON_DOWN,
	UI_EVENT_MOUSE_RIGHT_BUTTON_DOWN,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_DOWN,
	UI_EVENT_MOUSE_LEFT_BUTTON_UP,
	UI_EVENT_MOUSE_RIGHT_BUTTON_UP,
	UI_EVENT_MOUSE_MIDDLE_BUTTON_UP,
	UI_EVENT_KEY_DOWN,
	UI_EVENT_KEY_UP,
};

/**
*	\brief 将一个dispatcher链接到event handlder
*/
void ConnectDispatcher(Dispatcher* Dispatcher);

/**
*	\brief 将一个dispatcher从event handlder中解除链接
*/
void DisconnectDispatcher(Dispatcher* dispatcher);

}