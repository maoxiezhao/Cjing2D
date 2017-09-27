#ifndef _VIDEO_H_
#define _VIDEO_H_

#include"common\common.h"
#include"utils\size.h"

/**
*	\brief 初始化OpenGL，并绘制窗体
*/
class Video
{
public:
	static void Initialize();
	static void Quit();
	static bool IsInitialized();
	static void CleanCanvas();
	static void Rendercanvas();
	static bool IsExiting();
	static void SetTitleName(const string& tname);
	static string GetTitleName();
	static void SetFullScreen(bool fullscreen);
	static void SetWindowScreen();
	static bool IsFullScreen();
	static void SetFPS(uint32_t f);
	static uint32_t GetFPS();
	static const Size& GetScreenSize();
	static void SetImageCursor(unsigned char* imgData, int w, int h);
};

#endif