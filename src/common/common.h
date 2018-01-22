#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdint.h>
#include<string>
#include<memory>
#include<vector>
#include<iostream>
#include<windows.h>
#include<sstream>
#include<algorithm>

#define GLEW_STATIC
#include<glew.h>
#include<GLFW\glfw3.h>

using std::cout;
using std::endl;
using std::vector;
using std::string;

#ifdef _MSC_VER
	#pragma warning( disable :4005 4018 4522 4715 4800 4996)
#endif

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480

using wchar = unsigned int;

#define DEFAULT_APP_WRITE_DIR "Cjing"

#undef min
#undef max

#endif