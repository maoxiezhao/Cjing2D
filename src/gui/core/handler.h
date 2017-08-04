#pragma once

#include"common\common.h"

namespace gui
{

class Widget;
class Dispatcher;

enum ui_event
{
	UI_EVENT_UNKNOW
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