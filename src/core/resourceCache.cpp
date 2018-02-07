#include"resourceCache.h"
#include"logger.h"

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
	mFonts.clear();
}

/**
*	\brief 加载默认的着色器程序,目前的加载方式需要重构,散
*	文件过多，需要将所有着色器写在一个文件中解析
*/
void ResourceCache::LoadDefaultProgram()
{
	// 加载默认的normal sprite program
	auto normalProgram = std::make_shared<GLProgram>();
	normalProgram->InitWithFileNames(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".frag");
	normalProgram->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME] = normalProgram;

	// 记载默认的color sprite program
	auto colorProgram = std::make_shared<GLProgram>();
	colorProgram->InitWithFileNames(GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME + ".frag");
	colorProgram->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME] = colorProgram;

	// 加载默认的deferred geometry program
	auto deferredProgram = std::make_shared<GLProgram>();
	deferredProgram->InitWithFileNames(GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME + ".frag");
	deferredProgram->Link();
	mPrograms[GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME] = deferredProgram;

	// 加载默认的deferred light program
	auto deferredLightProgram = std::make_shared<GLProgram>();
	deferredLightProgram->InitWithFileNames(GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME + ".frag");
	deferredLightProgram->Link();
	mPrograms[GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME] = deferredLightProgram;

	// 记载默认的outlined sprite program
	auto outLined = std::make_shared<GLProgram>();
	outLined->InitWithFileNames(GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME + ".frag");
	outLined->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME] = outLined;

	// 加载默认的字体program
	auto fontProgram = std::make_shared<GLProgram>();
	fontProgram->InitWithFileNames(GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME + ".frag");
	fontProgram->Link();
	mPrograms[GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME] = fontProgram;

	// 加载默认的polygon program
	auto colorPolygonProgram = std::make_shared<GLProgram>();
	colorPolygonProgram->InitWithFileNames(GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME + ".frag");
	colorPolygonProgram->Link();
	mPrograms[GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME] = colorPolygonProgram;

	// 加载默认的post process program
	auto postProcessProgram = std::make_shared<GLProgram>();
	postProcessProgram->InitWithFileNames(GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME + ".frag");
	postProcessProgram->Link();
	mPrograms[GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME] = postProcessProgram;
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
*	 保证DefaultProgram已经加载好,这里分布加载normal_sprite和
*	 color_sprite
*/
void ResourceCache::LoadDefaultProgramState()
{
	// 加载默认的sprite program state
	auto newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_SPRITE_NORMAL_PROGRAMSTATE_NAME] = newProgramState;

	// 加载默认的color sprite program state 
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_SPRITE_COLOR_PROGRAMSTATE_NAME] = newProgramState;

	// 加载默认的deferred geometry program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_G_BUFFER_PROGRAMSTATE_NAME] = newProgramState;

	// 加载默认的deferred light program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_DEFERRED_LIGHT_PROGRAMSTATE_NAME] = newProgramState;

	// 加载默认的font program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_FONT_NORMAL_PROGRAMSTATE_NAME] = newProgramState;

	// 加载默认的polygon color program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_POLYGON_COLOR_PROGRAMSTATE_NAME] = newProgramState;

	// 加载默认的polygon post process program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_POST_PROCESS_PROGRAMSTATE_NAME] = newProgramState;
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

/**
*	\brief 创建一个指定大小的空白纹理
*/
TexturePtr ResourceCache::CreateTexture2DBySize(const string & texname, const Size & size)
{
	auto newTex = std::make_shared<Texture2D>();
	newTex->SetSize(size);
	newTex->InitWithChars(nullptr);
	mTextures[texname] = newTex;
	return newTex;
}

RenderTexturePtr ResourceCache::CreateRenderTexture2DBySize(const string & name, const Size & size)
{
	return RenderTexturePtr();
}


