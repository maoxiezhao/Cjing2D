#include "inputEvent.h"

std::set<InputEvent::KeyboardKey> InputEvent::mKeyPressed;
std::queue<InputEvent::KeyEvent> InputEvent::mEventQueue;

/**
*	\brief 获取当前的输入事件
*
*	事件从mEventQueue中获取，由checkInput()放置
*/
std::unique_ptr<InputEvent> InputEvent::GetEvent()
{
	InputEvent* result = nullptr;
	if (!mEventQueue.empty())
	{
		KeyEvent ent= mEventQueue.front();
		mEventQueue.pop();
		result = new InputEvent(ent);
	}
	return std::unique_ptr<InputEvent>(result);
}

/**
*	\brief 键盘输入回调函数，供glfw调用
*/
void InputEvent::key_callback(GLFWwindow * window, int key_in, int scancode, int action, int mode)
{
	KeyEvent ent;
	if (key_in >= 0 && key_in < 1024)
	{
		KeyboardKey key = static_cast<KeyboardKey>(key_in);
		ent.key = key;
		if (action == GLFW_PRESS)
		{	// 按键按下时，需要判断是否是一直按住的
			ent.repeat = false;
			ent.type = KEYDOWN;
			if (!mKeyPressed.insert(key).second)
				ent.repeat = 1;	// 重复按键
		}
		else if (action == GLFW_RELEASE)
		{
			ent.repeat = 0;
			ent.type = KEYUP;
			if (mKeyPressed.erase(key) == 0)
				ent.repeat = 1;	// 重复按键
		}
		else
		{
			ent.type = KEYDOWN;
			ent.repeat = 1;
		}
		mEventQueue.push(ent);
	}
}

InputEvent::InputEvent(const KeyEvent& ent) :
	mKeyEvent(ent)
{
}

/**
*	\brief 获取按键按下的按键
*
*	目前仅支持keyboard Event，所以当调用该函数时，必定存在按键被调用
*/
InputEvent::KeyboardKey InputEvent::GetKeyBoardKey() const
{
	return mKeyEvent.key;
}

/**
*	\brief 是否有键盘按键被按下
*/
bool InputEvent::IsKeyBoardPressed() const
{
	return (mKeyEvent.type == KEYDOWN) && (!mKeyEvent.repeat);
}

/**
*	\brief 是否有指定键盘按键被按下
*/
bool InputEvent::IsKeyBoardPressed(KeyboardKey key) const
{
	return IsKeyBoardPressed() && (GetKeyBoardKey() == key);
}

/**
*	\brief 是否有键盘按键被释放
*/
bool InputEvent::IsKeyBoardReleased() const
{
	return (mKeyEvent.type == KEYUP) && (!mKeyEvent.repeat);
}

/**
*	\brief 是否有指定键盘按键被释放
*/
bool InputEvent::IsKeyBoardReleased(KeyboardKey key) const
{
	return IsKeyBoardReleased() && (GetKeyBoardKey() == key);
}

/**
*	\brief 判断是否按下关闭窗口键
*
*	可更改，默认当按下Esc键时，令主循环跳出
*/
bool InputEvent::IsWindowClosing()const
{
	return (mKeyEvent.key == KEY_ESCAPE);
}