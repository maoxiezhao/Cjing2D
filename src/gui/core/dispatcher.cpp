#include "dispatcher.h"
#include"gui\core\handler.h"
#include"gui\core\dispatcher_private.h"
#include"gui\widget\widget.h"
#include"core\debug.h"

namespace gui
{

Dispatcher::Dispatcher() :
	mQueueSignal(),
	mQueueSignalMouse(),
	mQueueSignalKeyboard(),
	mIsConnected(false),
	mWantKeyboard(true),
	mMouseBehavior(all)
{
}

Dispatcher::~Dispatcher()
{
	if (mIsConnected)
		DisconnectDispatcher(this);
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
*	\brief ����һ��ͨ���¼�
*/
bool Dispatcher::Fire(const ui_event event, Widget & widget)
{
	if (Find<setEvent>(event, EventInSet()))
	{
		return	FireEvent<SignalFunction>(event, this, &widget);
	}
	return true;
}

bool Dispatcher::Fire(const ui_event event, Widget & widget, const Point2 & pos)
{
	if (Find<setEventMouse>(event, EventInSet()))
	{
		return FireEvent<SignalFunctionMouse>(event, this, &widget, pos);
	}
	return true;
}

bool Dispatcher::Fire(const ui_event event, Widget & widget, const InputEvent::KeyboardKey key, const string & unicode)
{
	if (Find<setEventKeyboard>(event, EventInSet()))
	{
		return FireEvent<SignalFunctionKeyboard>(event, this, &widget, key, unicode);
	}
	return true;
}

bool Dispatcher::Fire(const ui_event event, Widget & widget, Message & message)
{
	if (Find<setEventMessage>(event, EventInSet()) )
	{
		return FireEvent<SignalFunctionMessage>(event, this, &widget, message);
	}
	return true;
}

bool Dispatcher::Fire(const ui_event event, Widget & widget, void*data)
{
	if (Find<setEventNotification>(event, EventInSet()))
	{
		return	FireEvent<SignalFunctionNotification>(event, this, &widget, nullptr);
	}
	return true;
}

/**
*	\brief �ж��Ƿ���Բ���event
*	\param event �¼�����
*/
bool Dispatcher::HasEvent(const ui_event event, const event_queue_type type)
{
	return  Find<gui::setEvent>(event,         Dispatcher_implementation::HasHandler(type, *this)) ||
			Find<gui::setEventKeyboard>(event, Dispatcher_implementation::HasHandler(type, *this)) || 
			Find<gui::setEventMouse>(event,    Dispatcher_implementation::HasHandler(type, *this)) ||
		    Find<gui::setEventNotification>(event, Dispatcher_implementation::HasHandler(type, *this));
}

}
