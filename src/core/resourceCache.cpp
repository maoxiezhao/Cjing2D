#include"resourceCache.h"

ResourceCache::ResourceCache():
	mPrograms()
{
}

ResourceCache::~ResourceCache()
{
	Clear();
}

/**
*	\brief 返回ResourceCache唯一实例
*/
ResourceCache & ResourceCache::GetInstance()
{
	static ResourceCache instance;
	return instance;
}

void ResourceCache::Clear()
{
	mPrograms.clear();
	mProgramStates.clear();
	mTextures.clear();
}

/**
*	\brief 加载默认的着色器程序
*/
void ResourceCache::LoadDefaultProgram()
{
	// 加载默认的sprite program
	auto program = std::make_shared<GLProgram>();
	program->InitWithFileNames(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".frag");
	program->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME] = program;

	// 加载默认的字体program

}

/**
*	\brief 返回指定key的着色器程序
*
*  如果未找到，则返回nullptr
*/
void ResourceCache::AddGLProgram(GLProgramPtr ptr, const string & key)
{
	if (ptr == nullptr)
		return;
	mPrograms[key] = ptr;
}

/**
*	\brief 返回指定key的着色器程序
*
*  如果未找到，则返回nullptr
*/
GLProgramPtr ResourceCache::GetGLProgram(const string & key)
{
	auto& it = mPrograms.find(key);
	if (it != mPrograms.end())
		return it->second;
	return nullptr;
}

/**
*	\brief 加载默认的ProgramState
*
*	 保证DefaultProgram已经加载好
*/
void ResourceCache::LoadDefaultProgramState()
{
	// 加载默认的sprite program state
	auto newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_SPRITE_NORMAL_PROGRAMSTATE_NAME] = newProgramState;
}

/**
*	\brief 添加一个新的着色器状态
*
*   允许同名覆盖添加
*/
void ResourceCache::AddGLProgramState(GLProgramStatePtr ptr, const string & key)
{
	if (ptr == nullptr)
		return;
	mProgramStates[key] = ptr;
}

/**
*	\brief 返回指定key的着色器状态
*
*  如果未找到，则返回nullptr
*/
GLProgramStatePtr ResourceCache::GetGLProgramState(const string & key)
{
	auto& it = mProgramStates.find(key);
	if (it != mProgramStates.end())
		return it->second;
	return nullptr;
}

/**
*	\brief 加载一个纹理
*
*   不同名覆盖，即如果已经载入相同名字纹理，则直接返回对应纹理
*/
TexturePtr ResourceCache::LoadTexture2D(const string & texname)
{
	TexturePtr findTex = GetTexture2D(texname);
	if (findTex != nullptr)
		return findTex;

	auto newTex = std::make_shared<Texture2D>();
	newTex->InitWithFile(texname);
	mTextures[texname] = newTex;
	return newTex;
}

/**
*	\brief 返回指定key的纹理
*
*  如果未找到，则返回nullptr
*/
TexturePtr ResourceCache::GetTexture2D(const string & texnname)
{
	auto& it = mTextures.find(texnname);
	if (it != mTextures.end())
		return it->second;
	return nullptr;
}


