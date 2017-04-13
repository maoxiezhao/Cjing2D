#ifndef _INPUT_EVENT_H_
#define _INPUT_EVENT_H_

#include"common\common.h"
#include<GLFW\glfw3.h>
#include<set>
#include<queue>

/**
*	\brief ∑‚◊∞GLFW ‰»Î
*/
class InputEvent
{
public:
	enum KeyboardKey
	{
		KEY_NONE = GLFW_KEY_UNKNOWN,
		KEY_BACKSPACE = GLFW_KEY_BACKSPACE,
		KEY_TABULATION = GLFW_KEY_TAB,
		KEY_PAUSE = GLFW_KEY_PAUSE,
		KEY_ESCAPE = GLFW_KEY_ESCAPE,
		KEY_SPACE = GLFW_KEY_SPACE,
		KEY_COMMA = GLFW_KEY_COMMA,
		KEY_MINUS = GLFW_KEY_MINUS,
		KEY_PERIOD = GLFW_KEY_PERIOD,
		KEY_SLASH = GLFW_KEY_SLASH,
		KEY_0 = GLFW_KEY_0,
		KEY_1 = GLFW_KEY_1,
		KEY_2 = GLFW_KEY_2,
		KEY_3 = GLFW_KEY_3,
		KEY_4 = GLFW_KEY_4,
		KEY_5 = GLFW_KEY_5,
		KEY_6 = GLFW_KEY_6,
		KEY_7 = GLFW_KEY_7,
		KEY_8 = GLFW_KEY_8,
		KEY_9 = GLFW_KEY_9,
		KEY_SEMICOLON = GLFW_KEY_SEMICOLON,
		KEY_BACKSLASH = GLFW_KEY_BACKSLASH,

		KEY_a = GLFW_KEY_A,
		KEY_b = GLFW_KEY_B,
		KEY_c = GLFW_KEY_C,
		KEY_d = GLFW_KEY_D,
		KEY_e = GLFW_KEY_E,
		KEY_f = GLFW_KEY_F,
		KEY_g = GLFW_KEY_G,
		KEY_h = GLFW_KEY_H,
		KEY_i = GLFW_KEY_I,
		KEY_j = GLFW_KEY_J,
		KEY_k = GLFW_KEY_K,
		KEY_l = GLFW_KEY_L,
		KEY_m = GLFW_KEY_M,
		KEY_n = GLFW_KEY_N,
		KEY_o = GLFW_KEY_O,
		KEY_p = GLFW_KEY_P,
		KEY_q = GLFW_KEY_Q,
		KEY_r = GLFW_KEY_R,
		KEY_s = GLFW_KEY_S,
		KEY_t = GLFW_KEY_T,
		KEY_u = GLFW_KEY_U,
		KEY_v = GLFW_KEY_V,
		KEY_w = GLFW_KEY_W,
		KEY_x = GLFW_KEY_X,
		KEY_y = GLFW_KEY_Y,
		KEY_z = GLFW_KEY_Z,
		KEY_DELETE = GLFW_KEY_DELETE,

		KEY_KP0 = GLFW_KEY_KP_0,
		KEY_KP1 = GLFW_KEY_KP_1,
		KEY_KP2 = GLFW_KEY_KP_2,
		KEY_KP3 = GLFW_KEY_KP_3,
		KEY_KP4 = GLFW_KEY_KP_4,
		KEY_KP5 = GLFW_KEY_KP_5,
		KEY_KP6 = GLFW_KEY_KP_6,
		KEY_KP7 = GLFW_KEY_KP_7,
		KEY_KP8 = GLFW_KEY_KP_8,
		KEY_KP9 = GLFW_KEY_KP_9,
		KEY_KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
		KEY_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
		KEY_KP_ENTER = GLFW_KEY_KP_ENTER,

		KEY_UP = GLFW_KEY_UP,
		KEY_DOWN = GLFW_KEY_DOWN,
		KEY_RIGHT = GLFW_KEY_RIGHT,
		KEY_LEFT = GLFW_KEY_LEFT,
		KEY_INSERT = GLFW_KEY_INSERT,
		KEY_HOME = GLFW_KEY_HOME,
		KEY_END = GLFW_KEY_END,

		KEY_F1 = GLFW_KEY_F1,
		KEY_F2 = GLFW_KEY_F2,
		KEY_F3 = GLFW_KEY_F3,
		KEY_F4 = GLFW_KEY_F4,
		KEY_F5 = GLFW_KEY_F5,
		KEY_F6 = GLFW_KEY_F6,
		KEY_F7 = GLFW_KEY_F7,
		KEY_F8 = GLFW_KEY_F8,
		KEY_F9 = GLFW_KEY_F9,
		KEY_F10 = GLFW_KEY_F10,
		KEY_F11 = GLFW_KEY_F11,
		KEY_F12 = GLFW_KEY_F12,
		KEY_F13 = GLFW_KEY_F13,
		KEY_F14 = GLFW_KEY_F14,
		KEY_F15 = GLFW_KEY_F15
	};

	enum MouseButton
	{
		MOUSE_BUTTON_NOE = -1,
		MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_MIDDLE= GLFW_MOUSE_BUTTON_MIDDLE,
		MOUSE_BUTTON_RIGHT= GLFW_MOUSE_BUTTON_RIGHT
	};
	enum KeyType
	{
		UNKNOW,
		KEYDOWN,
		KEYUP
	};
	struct KeyEvent
	{
		KeyboardKey key;
		KeyType type;
		bool repeat;
		KeyEvent() :key(KEY_NONE), type(UNKNOW), repeat(false) {}
	};

public:
	static void Initialize();
	static void Quit();

	static std::unique_ptr<InputEvent> GetEvent();
	// callback
	static void key_callback(GLFWwindow* window, int key_in, int scancode, int action, int mode);
//	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//	static void mouse_button_callback(GLFWwindow* window, int button, int action, int modes);

	// keyboard
	KeyboardKey GetKeyBoardKey()const;
	bool IsKeyBoardPressed() const;
	bool IsKeyBoardPressed(KeyboardKey key)const;

	bool IsKeyBoardReleased() const;
	bool IsKeyBoardReleased(KeyboardKey key)const;

	// mouse
	MouseButton GetMouseButton()const;

	// window event
	bool IsWindowClosing()const;

private:
	InputEvent(const KeyEvent& ent);

	static std::set<KeyboardKey> mKeyPressed;
	static std::queue<KeyEvent> mEventQueue;
	KeyEvent mKeyEvent;

};


#endif