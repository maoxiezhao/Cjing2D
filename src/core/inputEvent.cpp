#include "inputEvent.h"

std::set<InputEvent::KeyboardKey> InputEvent::mKeyPressed;
std::queue<InputEvent::KeyEvent> InputEvent::mEventQueue;

const string EnumInfoTraits<InputEvent::KeyboardKey>::prettyName = "keyboard key";
const EnumInfo<InputEvent::KeyboardKey>::nameType EnumInfoTraits<InputEvent::KeyboardKey>::names = 
{
	{InputEvent::KEY_NONE,""},
	{InputEvent::KEY_BACKSLASH,"backspace"},
	{InputEvent::KEY_TABULATION,"tab"},
	{InputEvent::KEY_PAUSE,"pause"},
	{InputEvent::KEY_ESCAPE,"esc"},
	{InputEvent::KEY_COMMA,"comma"},
	{InputEvent::KEY_MINUS,"minus"},
	{InputEvent::KEY_PERIOD,"period"},
	{InputEvent::KEY_SLASH,"slash"},
	{InputEvent::KEY_SEMICOLON,"semicolor"},
	{InputEvent::KEY_BACKSLASH,"backslash"},

	{InputEvent::KEY_0,"0" },
	{InputEvent::KEY_1,"1" },
	{InputEvent::KEY_2,"2" },
	{InputEvent::KEY_3,"3" },
	{InputEvent::KEY_4,"4" },
	{InputEvent::KEY_5,"5" },
	{InputEvent::KEY_6,"6" },
	{InputEvent::KEY_7,"7" },
	{InputEvent::KEY_8,"8" },
	{InputEvent::KEY_9,"9" },

	{InputEvent::KEY_a,"a" },
	{InputEvent::KEY_b,"b" },
	{InputEvent::KEY_c,"c" },
	{InputEvent::KEY_d,"d" },
	{InputEvent::KEY_e,"e" },
	{InputEvent::KEY_f,"f" },
	{InputEvent::KEY_g,"g" },
	{InputEvent::KEY_h,"h" },
	{InputEvent::KEY_i,"i" },
	{InputEvent::KEY_j,"j" },
	{InputEvent::KEY_k,"k" },
	{InputEvent::KEY_l,"l" },
	{InputEvent::KEY_m,"m" },
	{InputEvent::KEY_n,"n" },
	{InputEvent::KEY_o,"o" },
	{InputEvent::KEY_p,"p" },
	{InputEvent::KEY_q,"q" },
	{InputEvent::KEY_r,"r" },
	{InputEvent::KEY_s,"s" },
	{InputEvent::KEY_t,"t" },
	{InputEvent::KEY_u,"u" },
	{InputEvent::KEY_v,"v" },
	{InputEvent::KEY_w,"w" },
	{InputEvent::KEY_x,"x" },
	{InputEvent::KEY_y,"y" },
	{InputEvent::KEY_z,"z" },
	{InputEvent::KEY_DELETE,"delete"},
	{InputEvent::KEY_KP_ENTER ,"enter"},

	{InputEvent::KEY_KP0,"kp0"},
	{InputEvent::KEY_KP1,"kp1"},
	{InputEvent::KEY_KP2,"kp2"},
	{InputEvent::KEY_KP3,"kp3"},
	{InputEvent::KEY_KP4,"kp4"},
	{InputEvent::KEY_KP5,"kp5"},
	{InputEvent::KEY_KP6,"kp6" },
	{InputEvent::KEY_KP7,"kp7" },
	{InputEvent::KEY_KP8,"kp8" },
	{InputEvent::KEY_KP9,"kp9" },
	{InputEvent::KEY_KP_DIVIDE ,"kpdivide" },
	{InputEvent::KEY_KP_MULTIPLY ,"kpmutiply" },
	{InputEvent::KEY_KP_ENTER ,"kpenter" },

	{InputEvent::KEY_UP,"up"},
	{InputEvent::KEY_DOWN,"down" },
	{InputEvent::KEY_RIGHT,"right" },
	{InputEvent::KEY_LEFT,"left" },
	{InputEvent::KEY_INSERT,"insert" },
	{InputEvent::KEY_HOME,"home" },
	{InputEvent::KEY_END,"end" },

	{InputEvent::KEY_F1,"f1" },
	{InputEvent::KEY_F2,"f2" },
	{InputEvent::KEY_F3,"f3" },
	{InputEvent::KEY_F4,"f4" },
	{InputEvent::KEY_F5,"f5" },
	{InputEvent::KEY_F6,"f6" },
	{InputEvent::KEY_F7,"f7" },
	{InputEvent::KEY_F8,"f8" },
	{InputEvent::KEY_F9,"f9" },
	{InputEvent::KEY_F10,"f10" },
	{InputEvent::KEY_F11,"f11" },
	{InputEvent::KEY_F12,"f12" },
	{InputEvent::KEY_F13,"f13" },
	{InputEvent::KEY_F14,"f14" },
	{InputEvent::KEY_F15,"f15" },

	{InputEvent::KEY_SHIFT,"shift" },
	{InputEvent::KEY_CTRL,"ctrl" },
	{InputEvent::KEY_ALT ,"alt" },
	
};


void InputEvent::Initialize()
{
}

void InputEvent::Quit()
{
}

bool InputEvent::IsInitialized()
{
	return false;
}

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

InputEvent::KeyEvent InputEvent::GetKeyEvent()const
{
	return mKeyEvent;
}

bool InputEvent::IsKeyBoardEvent() const
{
	return mKeyEvent.type == KEYBOARD_INPUT_TYPE;
}

bool InputEvent::IsMouseEvent() const
{
	return mKeyEvent.type == MOUSE_INPUT_TYPE;
}

/**
*	\brief 按键是否持续按下
*/
bool InputEvent::IsKeyBoardRepeatPressed(KeyboardKey key) const
{
	return mKeyPressed.find(key) != mKeyPressed.end();
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
		ent.type = KEYBOARD_INPUT_TYPE;
		if (action == GLFW_PRESS)
		{	// 按键按下时，需要判断是否是一直按住的
			ent.repeat = false;
			ent.state = KEYDOWN;
			if (!mKeyPressed.insert(key).second)
				ent.repeat = 1;	// 重复按键
		}
		else if (action == GLFW_RELEASE)
		{
			ent.repeat = 0;
			ent.state = KEYUP;
			if (mKeyPressed.erase(key) == 0)
				ent.repeat = 1;	// 重复按键
		}
		else
		{
			ent.state = KEYDOWN;
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
	return (mKeyEvent.state == KEYDOWN) && (!mKeyEvent.repeat);
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
	return (mKeyEvent.state == KEYUP) && (!mKeyEvent.repeat);
}

/**
*	\brief 是否有指定键盘按键被释放
*/
bool InputEvent::IsKeyBoardReleased(KeyboardKey key) const
{
	return IsKeyBoardReleased() && (GetKeyBoardKey() == key);
}

/**
*	\brief 当输入事件发生时是否按住ctrl
*/
bool InputEvent::IsWithKeyCtrl() const
{
	return  IsKeyBoardEvent() && IsKeyBoardRepeatPressed(KEY_CTRL);
}

/**
*	\brief 当输入事件发生时是否按住alt
*/
bool InputEvent::IsWithKeyAlt() const
{
	return  IsKeyBoardEvent() && IsKeyBoardRepeatPressed(KEY_ALT);
}

/**
*	\brief 当输入事件发生时是否按住shift
*/
bool InputEvent::IsWithKeyShift() const
{
	return  IsKeyBoardEvent() && IsKeyBoardRepeatPressed(KEY_SHIFT);
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