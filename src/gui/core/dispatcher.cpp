#include "dispatcher.h"
#include"gui\core\handler.h"
#include"gui\widget\widget.h"

namespace gui
{

Dispatcher::Dispatcher() :
	mSignalQueue(),
	mSignalMouseQueue(),
	mIsConnected(false)
{
}

Dispatcher::~Dispatcher()
{
	if (mIsConnected)
	{

	}
}

void Dispatcher::Connect()
{
	if (!mIsConnected)
	{
		mIsConnected = true;

	}
}

/**
*	\brief
*/
void Dispatcher::Fire(const ui_event event, Widget & widget)
{
}

}
