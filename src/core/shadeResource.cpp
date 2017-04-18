#include "shadeResource.h"

ShadeResourceCache::ShadeResourceCache():
	mPrograms()
{
}

ShadeResourceCache::~ShadeResourceCache()
{
	mPrograms.clear();
}

/**
*	\brief 返回ShadeResourceCache唯一实例
*/
ShadeResourceCache & ShadeResourceCache::GetInstance()
{
	static ShadeResourceCache instance;
	return instance;
}

/**
*	\brief 加载默认的着色器程序
*/
void ShadeResourceCache::LoadDefaultProgram()
{
	// 加载默认的sprite program
	auto program = std::make_shared<GLProgram>();
	program->InitWithFileNames(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".vert",
		GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".frag");
	mPrograms[GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME] = program;

	// 加载默认的字体program

}

/**
*	\brief 返回指定key的着色器程序
*
*  如果未找到，则返回nullptr
*/
GLProgramPtr ShadeResourceCache::GetGLProgram(const string & key)
{
	auto& it = mPrograms.find(key);
	if (it != mPrograms.end())
		return it->second;
	return nullptr;
}


