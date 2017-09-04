#include "handler.h"
#include"core\debug.h"
#include"core\inputEvent.h"
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
	void HandleEvent(const InputEvent& event);

	Dispatcher* GetKeyboardDispathcer();

	/** 触发各类事件 */
	void Keyboard(const ui_event event, const InputEvent::KeyboardKey key, const string& unicode);
	void KeyBoardKeyDown(const InputEvent& event);
	void KeyBoardKeyUp(const InputEvent& event);

	void Mouse(const ui_event event, const Point2& pos);
	void MouseButtonDown(const Point2& pos, const InputEvent::MouseButton button);
	void MouseButtonUp(const Point2& pos, const InputEvent::MouseButton button);

	void Draw();

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

/**
*	\brief 处理分发事件
*/
void EventHandler::HandleEvent(const InputEvent& event)
{
	InputEvent::KeyEvent keyEvent = event.GetKeyEvent();
	switch (keyEvent.type)
	{
	case InputEvent::EVENT_KEYBOARD_KEYDOWN:
		KeyBoardKeyDown(event);
		break;
	case InputEvent::EVENT_KEYBOARD_KEYUP:
		KeyBoardKeyUp(event);
		break;
	case InputEvent::EVENT_MOUSE_BUTTONDOWN:
		MouseButtonDown({ keyEvent.motion.x, keyEvent.motion.y }, keyEvent.mousebutton);
		break;
	case InputEvent::EVENT_MOUSE_BUTTONUP:
		MouseButtonUp({ keyEvent.motion.x, keyEvent.motion.y }, keyEvent.mousebutton);
		break;
	case InputEvent::EVENT_MOUSE_MOTION:
		Mouse(UI_EVENT_MOUSE_MOTION, { keyEvent.motion.x, keyEvent.motion.y });
		break;
	case InputEvent::EVENT_DRAW:
		Draw();
		break;
	default:
		break;
	}
}

/**
*	\brief 获取能够监听键盘输入的dispathcer
*
*	键盘监听事件为抢断式，从最外从开始拦获事件
*/
Dispatcher * EventHandler::GetKeyboardDispathcer()
{
	std::vector<Dispatcher*> reverseDispatchers = mDispatcher;
	std::reverse(reverseDispatchers.begin(), reverseDispatchers.end());

	for (auto& dispatcher : reverseDispatchers)
	{
		if (dispatcher->GetWantKeyboard())
		{
			return dispatcher;
		}
	}

	return nullptr;
}

void EventHandler::Keyboard(const ui_event event, const InputEvent::KeyboardKey key, const string & unicode)
{
	Dispatcher* dispathcer = GetKeyboardDispathcer();
	if (dispathcer)
	{
		dispathcer->Fire(event, dynamic_cast<Widget&>(*dispathcer), key, unicode);
	}
}

/**
*	\brief 键盘按下事件
*/
void EventHandler::KeyBoardKeyDown(const InputEvent& event)
{
	InputEvent::KeyboardKey key = event.GetKeyBoardKey();
	const string unicode = EnumToString(key, "");
	Keyboard(UI_EVENT_KEY_DOWN, key, unicode);
}

/**
*	\brief 键盘松开事件
*/
void EventHandler::KeyBoardKeyUp(const InputEvent& event)
{
	InputEvent::KeyboardKey key = event.GetKeyBoardKey();
	const string unicode = EnumToString(key, "");
	Keyboard(UI_EVENT_KEY_UP, key, unicode);
}

/**
*	\brief 鼠标事件处理
*/
void EventHandler::Mouse(const ui_event event, const Point2& pos)
{
	std::vector<Dispatcher*> reverseDispatchers = mDispatcher;
	std::reverse(reverseDispatchers.begin(), reverseDispatchers.end());

	for (auto& dispatcher : reverseDispatchers)
	{
		if (dispatcher->GetMouseBehavior() == Dispatcher::all)
		{
			dispatcher->Fire(event, dynamic_cast<Widget&>(*dispatcher), pos);
			break;
		}

		if (dispatcher->GetMouseBehavior() == Dispatcher::none)
		{
			continue;
		}

		// 如果widget在pos中，则触发事件
		if (dispatcher->IsAt(pos))
		{
			dispatcher->Fire(event, dynamic_cast<Widget&>(*dispatcher), pos);
			break;
		}
	}
}

/**
*	\brief 鼠标按下事件
*/
void EventHandler::MouseButtonDown(const Point2& pos, const InputEvent::MouseButton button)
{
	switch (button)
	{
	case InputEvent::MOUSE_BUTTON_LEFT:
		Mouse(UI_EVENT_MOUSE_LEFT_BUTTON_DOWN, pos);
		break;
	case InputEvent::MOUSE_BUTTON_MIDDLE:
		Mouse(UI_EVENT_MOUSE_MIDDLE_BUTTON_DOWN, pos);
		break;
	case InputEvent::MOUSE_BUTTON_RIGHT:
		Mouse(UI_EVENT_MOUSE_RIGHT_BUTTON_DOWN, pos);
		break;
	}
}

/**
*	\brief 鼠标松开事件
*/
void EventHandler::MouseButtonUp(const Point2& pos, const InputEvent::MouseButton button)
{
	switch (button)
	{
	case InputEvent::MOUSE_BUTTON_LEFT:
		Mouse(UI_EVENT_MOUSE_LEFT_BUTTON_UP, pos);
		break;
	case InputEvent::MOUSE_BUTTON_MIDDLE:
		Mouse(UI_EVENT_MOUSE_MIDDLE_BUTTON_UP, pos);
		break;
	case InputEvent::MOUSE_BUTTON_RIGHT:
		Mouse(UI_EVENT_MOUSE_RIGHT_BUTTON_UP, pos);
		break;
	}
}

/**
*	\brief 绘制事件
*/
void EventHandler::Draw()
{
	for (auto& dispatcher : mDispatcher)
	{	
		dispatcher->Fire(ui_event::UI_EVENT_DRAW, dynamic_cast<Widget&>(*dispatcher));
	}
}

/************ 全局函数 *************/
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

/************ GUI MANAGER *************/
GUIManager::GUIManager()
{
	mHandler.reset(new EventHandler());
}

GUIManager::~GUIManager()
{
	mHandler.reset(nullptr);
}

void GUIManager::HandleEvent(const InputEvent& event)
{
	if (mHandler != nullptr)
	{
		mHandler->HandleEvent(event);
	}
}



}