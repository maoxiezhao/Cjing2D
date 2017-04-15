#include "video.h"
#include "debug.h"
#include "inputEvent.h"
#include "utils\Size.h"

namespace
{
	GLFWwindow*  mainWindow = nullptr;
	GLFWmonitor* mainMonitor = nullptr;
	bool fullScreenWindow = false;
	bool visibleCursor = true;
	bool initialized = false;
	Size wantedWindowSize;
	Size positionWindow;
	string tilename;
}

/**
*	\brief 初始化video模块
*
*	初始化openGL
*/
void Video::Initialize()
{
	Debug::CheckAssertion(!IsInitialized(), "The video already initialized.");
	wantedWindowSize = {DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT};

	// 初始化glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// opengl核心模式
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	tilename = "CJING v0.1";
	mainMonitor = glfwGetPrimaryMonitor();
	mainWindow = glfwCreateWindow(wantedWindowSize.width, wantedWindowSize.height, tilename.c_str(), fullScreenWindow ? mainMonitor : nullptr, nullptr);
	Debug::CheckAssertion(mainWindow != nullptr, "Cannot create window.");

	// 移动窗口到中心位置
	positionWindow.width = (int)((glfwGetVideoMode(mainMonitor)->width  - wantedWindowSize.width)  * 0.5f);
	positionWindow.height = (int)((glfwGetVideoMode(mainMonitor)->height - wantedWindowSize.height) * 0.5f);
	glfwSetWindowPos(mainWindow, positionWindow.width, positionWindow.height);
	glfwMakeContextCurrent(mainWindow);

	// 设置输入回调函数
	Debug::CheckAssertion(!InputEvent::IsInitialized(), "The input must Initialized before video.");
	glfwSetKeyCallback(mainWindow, InputEvent::key_callback);
	if(!visibleCursor)// shadow the cursor
		glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	
	//glfwSetCursorPosCallback(window, InputEvent::mouse_callback);
	//glfwSetMouseButtonCallback(window, InputEvent::mouse_button_callback);

	// 初始化glew
	glewInit();
	glViewport(0, 0, wantedWindowSize.width, wantedWindowSize.height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initialized = true;
}

void Video::Quit()
{
	if (!IsInitialized())
		return;
	if (mainWindow != nullptr)
		glfwTerminate();
}

bool Video::IsInitialized()
{
	return initialized;
}

bool Video::IsExiting()
{
	return glfwWindowShouldClose(mainWindow);
}

void Video::CreateWindow()
{
}

void Video::SetTitleName(const string & tname)
{
	if (!IsInitialized())
		return;
	glfwSetWindowTitle(mainWindow,tname.c_str());
	tilename = tname;
}

string Video::GetTitleName()
{
	return tilename;
}

/**
*	\brief 设置为全屏模式
*/
void Video::SetFullScreen(bool fullscreen)
{
	Debug::CheckAssertion(IsInitialized(), "No Initialized.");
	fullScreenWindow = fullscreen;
	if (fullScreenWindow)
	{
		glfwSetWindowPos(mainWindow, 0, 0);
		glfwSetWindowMonitor(mainWindow, mainMonitor, 0, 0, wantedWindowSize.width, wantedWindowSize.height, glfwGetVideoMode(mainMonitor)->refreshRate);
	}
	else
	{
		glfwSetWindowPos(mainWindow, positionWindow.width, positionWindow.height);
		glfwSetWindowMonitor(mainWindow, nullptr, positionWindow.width, positionWindow.height, wantedWindowSize.width, wantedWindowSize.height, glfwGetVideoMode(mainMonitor)->refreshRate);
	}
}

/**
*	\brief 设置为窗体模式
*/
void Video::SetWindowScreen()
{
	Debug::CheckAssertion(IsInitialized(), "No Initialized.");
	glfwSetWindowMonitor(mainWindow, nullptr, positionWindow.width, positionWindow.height, wantedWindowSize.width, wantedWindowSize.height, glfwGetVideoMode(mainMonitor)->refreshRate);

}

/**
*	\brief 是否是fullScreen
*/
bool Video::IsFullScreen()
{
	return fullScreenWindow;
}
