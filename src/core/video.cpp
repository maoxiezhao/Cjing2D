#include "video.h"
#include "debug.h"
#include "inputEvent.h"
#include "renderer.h"
#include "utils\Size.h"
#include "resourceCache.h"
#include "configData.h"

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
	Renderer* mRenderer = nullptr;
	uint32_t fps = 0;
	GLFWimage cursorImage;
	GLFWcursor* cursor = nullptr;
}

/**
*	\brief ��ʼ��videoģ��
*
*	��ʼ��openGL
*/
void Video::Initialize()
{
	Debug::CheckAssertion(!IsInitialized(), "The video already initialized.");
	auto& configProperties = ConfigData::GetConfigProperties();
	wantedWindowSize = configProperties.GetNormalSize();

	// ��ʼ��glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// opengl����ģʽ
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	tilename = configProperties.GetTitle();
	mainMonitor = glfwGetPrimaryMonitor();
	mainWindow = glfwCreateWindow(wantedWindowSize.width, wantedWindowSize.height, tilename.c_str(), fullScreenWindow ? mainMonitor : nullptr, nullptr);
	Debug::CheckAssertion(mainWindow != nullptr, "Cannot create window.");

	// �ƶ����ڵ�����λ��
	positionWindow.width = (int)((glfwGetVideoMode(mainMonitor)->width  - wantedWindowSize.width)  * 0.5f);
	positionWindow.height = (int)((glfwGetVideoMode(mainMonitor)->height - wantedWindowSize.height) * 0.5f);
	glfwSetWindowPos(mainWindow, positionWindow.width, positionWindow.height);
	glfwMakeContextCurrent(mainWindow);

	// ��������ص�����
	Debug::CheckAssertion(!InputEvent::IsInitialized(), "The input must Initialized before video.");
	glfwSetKeyCallback(mainWindow, InputEvent::key_callback);

	if(!visibleCursor)// shadow the cursor
		glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	
	glfwSetCursorPosCallback(mainWindow, InputEvent::mouse_motion_callback);
	glfwSetMouseButtonCallback(mainWindow, InputEvent::mouse_button_callback);

	// ��ʼ��glew
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, wantedWindowSize.width, wantedWindowSize.height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_POINT_SMOOTH);				// �����
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	initialized = true;

	// ��ʼ����Ⱦģ��
	mRenderer = &Renderer::GetInstance();
	mRenderer->Initialize(wantedWindowSize.width,wantedWindowSize.height);

	// ��ʼ����ɫ��Դģ��
	ResourceCache::GetInstance().LoadDefaultProgram();
	ResourceCache::GetInstance().LoadDefaultProgramState();
}

void Video::Quit()
{
	if (!IsInitialized())
		return;
	ResourceCache::GetInstance().Clear();
	Renderer::GetInstance().Quit();
	if (mainWindow != nullptr)
		glfwTerminate();
}

bool Video::IsInitialized()
{
	return initialized;
}

/**
*	\brief ���������Ĭ�����ý���Ϊ��ɫ
*/
void Video::CleanCanvas()
{
	mRenderer->RenderClear();
}

/**
*	\brief ���ƻ���
*
*	�ȵ���Renderer��render����ִ�н�������
*/
void Video::Rendercanvas()
{
	if (!IsInitialized())
		return;
	mRenderer->Render();
	glfwSwapBuffers(mainWindow);
}

bool Video::IsExiting()
{
	return glfwWindowShouldClose(mainWindow);
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
*	\brief ����Ϊȫ��ģʽ
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
*	\brief ����Ϊ����ģʽ
*/
void Video::SetWindowScreen()
{
	Debug::CheckAssertion(IsInitialized(), "No Initialized.");
	glfwSetWindowMonitor(mainWindow, nullptr, positionWindow.width, positionWindow.height, wantedWindowSize.width, wantedWindowSize.height, glfwGetVideoMode(mainMonitor)->refreshRate);

}

/**
*	\brief �Ƿ���fullScreen
*/
bool Video::IsFullScreen()
{
	return fullScreenWindow;
}

void Video::SetFPS(uint32_t f)
{
	fps = f;
}

uint32_t Video::GetFPS()
{
	return fps;
}

/**
*	\brief ���ص�ǰ��Ļ�����С
*/
const Size & Video::GetScreenSize()
{
	return wantedWindowSize;
}

/**
*	\brief ���õ�ǰ���Ĺ��
*	\param imageData,32-bit, little-endian, 
*		   non-premultiplied RGBA
*/
void Video::SetImageCursor(unsigned char * imgData, int w, int h)
{
	cursorImage.width = w;
	cursorImage.height = h;
	cursorImage.pixels = imgData;

	if (cursor != nullptr)
	{
		glfwDestroyCursor(cursor);
		cursor = nullptr;
	}
	cursor = glfwCreateCursor(&cursorImage, 0, 0);
	glfwSetCursor(mainWindow, cursor);
}
