#ifndef _VIDEO_H_
#define _VIDEO_H_

#include"common\common.h"

/**
*	\brief 初始化OpenGL，并绘制窗体
*/
class Video
{
public:
	static void Initialize();
	static void Quit();
	static bool IsInitialized();
	static bool IsExiting();
	static void CreateWindow();
	static void SetTitleName(const string& tname);
	static string GetTitleName();
	static void SetFullScreen();
	static void SetWindowScreen();
};

#endif