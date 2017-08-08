#include "dispatcher.h"
#include"gui\core\handler.h"
#include"gui\core\dispatcher_private.h"
#include"gui\widget\widget.h"

namespace gui
{

Dispatcher::Dispatcher() :
	mQueueSignal(),
	mQueueSignalMouse(),
	mQueueSignalKeyboard(),
	mIsConnected(false),
	mWantKeyboard(false),
	mMouseBehavior(none)
{
}

Dispatcher::~Dispatcher()
{
	if (mIsConnected)
	{
		DisconnectDispatcher(this);
	}
}

void Dispatcher::Connect()
{
	if (!mIsConnected)
	{
		mIsConnected = true;
		ConnectDispatcher(this);
	}
}

bool Dispatcher::IsAt(const Point2 & pos)const
{
	return false;
}

class EventInSet
{
public:
	template<typename T>
	bool operator()(ui_event event)
	{
		return true;
	}
};

/**
*	\brief
*/
void Dispatcher::Fire(const ui_event event, Widget & widget)
{
	if (Find<setEvent>(event, EventInSet()))
	{
		FireEvent<SignalFunction>(event, this, &widget);
	}
}

void Dispatcher::Fire(const ui_event event, Widget & widget, const Point2 & pos)
{
	if (Find<setEventMouse>(event, EventInSet()))
	{
		FireEvent<SignalFunctionMouse>(event, this, &widget, pos);
	}
}

void Dispatcher::Fire(const ui_event event, Widget & widget, const InputEvent::KeyboardKey key, const string & unicode)
{
	if (Find<setEventKeyboard>(event, EventInSet()))
	{
		FireEvent<SignalFunctionKeyboard>(event, this, &widget, key, unicode);
	}
	
}

/**
*	\brief 判断是否可以操作event
*	\param event 事件类型
*/
bool Dispatcher::HasEvent(const ui_event event, const event_queue_type type)
{
	return  Find<gui::setEvent>(event, Dispatcher_implementation::HasHandler(type, *this)) ||
			Find<gui::setEventKeyboard>(event, Dispatcher_implementation::HasHandler(type, *this)) || 
			Find<gui::setEventMouse>(event, Dispatcher_implementation::HasHandler(type, *this)) ;
}

}
