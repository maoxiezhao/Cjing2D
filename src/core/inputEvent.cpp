#include "inputEvent.h"

std::set<InputEvent::KeyboardKey> InputEvent::mKeyPressed;
std::set<InputEvent::MouseButton> InputEvent::mMousePressed;
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
	{InputEvent::KEY_ENTER, "enter"},
};

Point2 InputEvent::mMousePos = Point2(0, 0);

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
*	\brief ���ڻ�ȡ���¼������������������߼��������¼�
*/
std::unique_ptr<InputEvent> InputEvent::GetSingleEvent(const EventType type)
{
	KeyEvent ent;
	ent.type = type;
	InputEvent* result = new InputEvent(ent);
	return std::unique_ptr<InputEvent>(result);
}

/**
*	\brief ��ȡ��ǰ�������¼�
*
*	�¼���mEventQueue�л�ȡ����checkInput()����
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

Point2 InputEvent::GetMousePos()
{
	return mMousePos;
}

InputEvent::KeyEvent InputEvent::GetKeyEvent()const
{
	return mKeyEvent;
}

bool InputEvent::IsKeyBoardEvent() const
{
	return mKeyEvent.type == EVENT_KEYBOARD_KEYDOWN ||
		   mKeyEvent.type == EVENT_KEYBOARD_KEYUP;
}

bool InputEvent::IsMouseEvent() const
{
	return mKeyEvent.type == EVENT_MOUSE_MOTION ||
		   mKeyEvent.type == EVENT_MOUSE_BUTTONDOWN ||
		   mKeyEvent.type == EVENT_MOUSE_BUTTONUP;
}

/**
*	\brief �����Ƿ��������
*/
bool InputEvent::IsKeyBoardRepeatPressed(KeyboardKey key) const
{
	return mKeyPressed.find(key) != mKeyPressed.end();
}

/**
*	\brief ��������ص���������glfw����
*/
void InputEvent::key_callback(GLFWwindow * window, int key_in, int scancode, int action, int mode)
{
	KeyEvent ent;
	if (key_in >= 0 && key_in < 1024)
	{
		KeyboardKey key = static_cast<KeyboardKey>(key_in);
		ent.key = key;
		if (action == GLFW_PRESS)
		{	// ��������ʱ����Ҫ�ж��Ƿ���һֱ��ס��
			ent.repeat = false;
			ent.state = KEYDOWN;
			ent.type = EVENT_KEYBOARD_KEYDOWN;

			if (!mKeyPressed.insert(key).second)
				ent.repeat = 1;	// �ظ�����
		}
		else if (action == GLFW_RELEASE)
		{
			ent.repeat = 0;
			ent.state = KEYUP;
			ent.type = EVENT_KEYBOARD_KEYUP;

			if (mKeyPressed.erase(key) == 0)
				ent.repeat = 1;	// �ظ�����
		}
		else
		{
			ent.type = EVENT_KEYBOARD_KEYDOWN;
			ent.state = KEYDOWN;
			ent.repeat = 1;
		}
		mEventQueue.push(ent);
	}
}

/**
*	\brief ����ƶ��ص���������glfw����
*/
void InputEvent::mouse_motion_callback(GLFWwindow* window, double xpos, double ypos)
{
	mMousePos = Point2((int)xpos, (int)ypos);
	KeyEvent ent;
	ent.type = EVENT_MOUSE_MOTION;
	ent.motion = mMousePos;
	mEventQueue.push(ent);
}

/**
*	\brief ������ص���������glfw����
*/
void InputEvent::mouse_button_callback(GLFWwindow* window, int button, int action, int modes)
{
	if (button >= MOUSE_BUTTON_LEFT && button <= MOUSE_BUTTON_MIDDLE)
	{
		KeyEvent ent;
		ent.mousebutton = static_cast<MouseButton>(button);
		if (action == GLFW_PRESS)
		{	// ��������ʱ����Ҫ�ж��Ƿ���һֱ��ס��
			ent.repeat = false;
			ent.state = KEYDOWN;
			ent.type = EVENT_MOUSE_BUTTONDOWN;

			if (!mMousePressed.insert(ent.mousebutton).second)
				ent.repeat = 1;	// �ظ�����
		}
		else if (action == GLFW_RELEASE)
		{
			ent.repeat = 0;
			ent.state = KEYUP;
			ent.type = EVENT_MOUSE_BUTTONUP;

			if (mMousePressed.erase(ent.mousebutton) == 0)
				ent.repeat = 1;	// �ظ�����
		}
		else
		{
			ent.type = EVENT_MOUSE_BUTTONDOWN;
			ent.state = KEYDOWN;
			ent.repeat = 1;
		}
		ent.motion = mMousePos;
		mEventQueue.push(ent);
	}
}

InputEvent::InputEvent(const KeyEvent& ent) :
	mKeyEvent(ent)
{
}

/**
*	\brief ��ȡ�������µİ���
*
*	Ŀǰ��֧��keyboard Event�����Ե����øú���ʱ���ض����ڰ���������
*/
InputEvent::KeyboardKey InputEvent::GetKeyBoardKey() const
{
	return mKeyEvent.key;
}

/**
*	\brief �Ƿ��м��̰���������
*/
bool InputEvent::IsKeyBoardPressed() const
{
	return (mKeyEvent.state == KEYDOWN) && (!mKeyEvent.repeat);
}

/**
*	\brief �Ƿ���ָ�����̰���������
*/
bool InputEvent::IsKeyBoardPressed(KeyboardKey key) const
{
	return IsKeyBoardPressed() && (GetKeyBoardKey() == key);
}

/**
*	\brief �Ƿ��м��̰������ͷ�
*/
bool InputEvent::IsKeyBoardReleased() const
{
	return (mKeyEvent.state == KEYUP) && (!mKeyEvent.repeat);
}

/**
*	\brief �Ƿ���ָ�����̰������ͷ�
*/
bool InputEvent::IsKeyBoardReleased(KeyboardKey key) const
{
	return IsKeyBoardReleased() && (GetKeyBoardKey() == key);
}

/**
*	\brief �������¼�����ʱ�Ƿ�סctrl
*/
bool InputEvent::IsWithKeyCtrl() const
{
	return  IsKeyBoardEvent() && IsKeyBoardRepeatPressed(KEY_CTRL);
}

/**
*	\brief �������¼�����ʱ�Ƿ�סalt
*/
bool InputEvent::IsWithKeyAlt() const
{
	return  IsKeyBoardEvent() && IsKeyBoardRepeatPressed(KEY_ALT);
}

/**
*	\brief �������¼�����ʱ�Ƿ�סshift
*/
bool InputEvent::IsWithKeyShift() const
{
	return  IsKeyBoardEvent() && IsKeyBoardRepeatPressed(KEY_SHIFT);
}

InputEvent::MouseButton InputEvent::GetMouseButton() const
{
	return mKeyEvent.mousebutton;
}

/**
*	\brief �ж��Ƿ��¹رմ��ڼ�
*
*	�ɸ��ģ�Ĭ�ϵ�����Esc��ʱ������ѭ������
*/
bool InputEvent::IsWindowClosing()const
{
	return (mKeyEvent.key == KEY_ESCAPE);
}

InputEvent::EventType InputEvent::GetEventType()const
{
	return mKeyEvent.type;
}