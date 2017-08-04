#include "handler.h"
#include"core\debug.h"
#include"gui\widget\widget.h"
#include"gui\core\dispatcher.h"

namespace gui
{

static std::unique_ptr<class EventHandler> mHandler = nullptr;

/**
*	\brief 事件处理者
*
*	event hander维护链接的Dispatcher，同时接受inputevent发来的事件，同时
*	将事件分发给链接了的dispatcher
*/
class EventHandler
{
public:
	EventHandler();
	~EventHandler();

	void Connect(Dispatcher* dispatcher);
	void Disconnect(Dispatcher* Dispatcher);

	void Active();

private:
	std::vector<Dispatcher*> mDispatcher;

};

EventHandler::EventHandler():
	mDispatcher()
{
}

EventHandler::~EventHandler()
{
	mDispatcher.clear();
}

/**
*	\brief 链接dispatcher,Dispatcher必须保证唯一性
*/
void EventHandler::Connect(Dispatcher * dispatcher)
{
	Debug::CheckAssertion(std::find(mDispatcher.begin(), mDispatcher.end(), dispatcher) == mDispatcher.end(),
		"dispatcher had already connected. ");

	mDispatcher.push_back(dispatcher);
}

void EventHandler::Disconnect(Dispatcher * Dispatcher)
{
	auto itr = std::find(mDispatcher.begin(), mDispatcher.end(), Dispatcher);
	Debug::CheckAssertion(itr != mDispatcher.end());

	mDispatcher.erase(itr);

	/** 当删除一个widget，对所有widget分发active事件 */
	Active();
}

void EventHandler::Active()
{
	for (auto dispatcher: mDispatcher)
	{
		//dispatcher->Fire(UI_EVENT_UNKNOW, std::<Widget&>(*dispatcher));
	}
}

/***** 全局函数 *****/
void ConnectDispatcher(Dispatcher * dispatcher)
{
	Debug::CheckAssertion(mHandler != nullptr, 
		"event handler have not exist.");
	Debug::CheckAssertion(dispatcher != nullptr, 
		"the Dispatcher connect to event handler is null.");
	mHandler->Connect(dispatcher);
}

void DisconnectDispatcher(Dispatcher * dispatcher)
{
	Debug::CheckAssertion(mHandler != nullptr, 
		"event handler have not exist.");
	Debug::CheckAssertion(dispatcher != nullptr, 
		"the Dispatcher disconnect to event handler is null.");
	mHandler->Disconnect(dispatcher);
}


}