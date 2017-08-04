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
*	\brief ��һ��dispatcher���ӵ�event handlder
*/
void ConnectDispatcher(Dispatcher* Dispatcher);

/**
*	\brief ��һ��dispatcher��event handlder�н������
*/
void DisconnectDispatcher(Dispatcher* dispatcher);

}