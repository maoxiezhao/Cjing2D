#include "dispatcher.h"
#include"gui\core\handler.h"
#include"gui\widget\widget.h"

namespace gui
{

Dispatcher::Dispatcher() :
	mSignalQueue(),
	mSignalMouseQueue(),
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

	}
}

bool Dispatcher::IsAt(const Point2 & pos)
{
	return false;
}

/**
*	\brief
*/
void Dispatcher::Fire(const ui_event event, Widget & widget)
{
}

void Dispatcher::Fire(const ui_event event, Widget & widget, const Point2 & pos)
{
}

void Dispatcher::Fire(const ui_event event, Widget & widget, const InputEvent::KeyboardKey key, const string & unicode)
{
}

}
