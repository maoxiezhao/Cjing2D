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
*	\brief ����ResourceCacheΨһʵ��
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
*	\brief ����Ĭ�ϵ���ɫ������,Ŀǰ�ļ��ط�ʽ��Ҫ�ع�,ɢ
*	�ļ����࣬��Ҫ��������ɫ��д��һ���ļ��н���
*/
void ResourceCache::LoadDefaultProgram()
{
	// ����Ĭ�ϵ�normal sprite program
	auto normalProgram = std::make_shared<GLProgram>();
	normalProgram->InitWithFileNames(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME + ".frag");
	normalProgram->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME] = normalProgram;

	// ����Ĭ�ϵ�color sprite program
	auto colorProgram = std::make_shared<GLProgram>();
	colorProgram->InitWithFileNames(GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME + ".frag");
	colorProgram->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME] = colorProgram;

	// ����Ĭ�ϵ�deferred geometry program
	auto deferredProgram = std::make_shared<GLProgram>();
	deferredProgram->InitWithFileNames(GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME + ".frag");
	deferredProgram->Link();
	mPrograms[GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME] = deferredProgram;

	// ����Ĭ�ϵ�deferred light program
	auto deferredLightProgram = std::make_shared<GLProgram>();
	deferredLightProgram->InitWithFileNames(GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME + ".frag");
	deferredLightProgram->Link();
	mPrograms[GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME] = deferredLightProgram;

	// ����Ĭ�ϵ�outlined sprite program
	auto outLined = std::make_shared<GLProgram>();
	outLined->InitWithFileNames(GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME + ".frag");
	outLined->Link();
	mPrograms[GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME] = outLined;

	// ����Ĭ�ϵ�����program
	auto fontProgram = std::make_shared<GLProgram>();
	fontProgram->InitWithFileNames(GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME + ".frag");
	fontProgram->Link();
	mPrograms[GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME] = fontProgram;

	// ����Ĭ�ϵ�polygon program
	auto colorPolygonProgram = std::make_shared<GLProgram>();
	colorPolygonProgram->InitWithFileNames(GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME + ".frag");
	colorPolygonProgram->Link();
	mPrograms[GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME] = colorPolygonProgram;

	// ����Ĭ�ϵ�post process program
	auto postProcessProgram = std::make_shared<GLProgram>();
	postProcessProgram->InitWithFileNames(GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME + ".vs",
		GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME + ".frag");
	postProcessProgram->Link();
	mPrograms[GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME] = postProcessProgram;
}

/**
*	\brief ����ָ��key����ɫ������
*
*  ���δ�ҵ����򷵻�nullptr
*/
void ResourceCache::AddGLProgram(GLProgramPtr ptr, const string & key)
{
	if (ptr == nullptr)
		return;
	mPrograms[key] = ptr;
}

/**
*	\brief ����ָ��key����ɫ������
*
*  ���δ�ҵ����򷵻�nullptr
*/
GLProgramPtr ResourceCache::GetGLProgram(const string & key)
{
	auto& it = mPrograms.find(key);
	if (it != mPrograms.end())
		return it->second;
	return nullptr;
}

/**
*	\brief ����Ĭ�ϵ�ProgramState
*
*	 ��֤DefaultProgram�Ѿ����غ�,����ֲ�����normal_sprite��
*	 color_sprite
*/
void ResourceCache::LoadDefaultProgramState()
{
	// ����Ĭ�ϵ�sprite program state
	auto newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_SPRITE_NORMAL_PROGRAMSTATE_NAME] = newProgramState;

	// ����Ĭ�ϵ�color sprite program state 
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_SPRITE_COLOR_PROGRAMSTATE_NAME] = newProgramState;

	// ����Ĭ�ϵ�deferred geometry program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_G_BUFFER_PROGRAMSTATE_NAME] = newProgramState;

	// ����Ĭ�ϵ�deferred light program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_DEFERRED_LIGHT_PROGRAMSTATE_NAME] = newProgramState;

	// ����Ĭ�ϵ�font program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_FONT_NORMAL_PROGRAMSTATE_NAME] = newProgramState;

	// ����Ĭ�ϵ�polygon color program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_POLYGON_COLOR_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_POLYGON_COLOR_PROGRAMSTATE_NAME] = newProgramState;

	// ����Ĭ�ϵ�polygon post process program state
	newProgramState = std::make_shared<GLProgramState>();
	newProgramState->Set(GetGLProgram(GLProgram::DEFAULT_POST_PROCESS_PROGRAM_NAME));
	mProgramStates[GLProgramState::DEFAULT_POST_PROCESS_PROGRAMSTATE_NAME] = newProgramState;
}

/**
*	\brief ���һ���µ���ɫ��״̬
*
*   ����ͬ���������
*/
void ResourceCache::AddGLProgramState(GLProgramStatePtr ptr, const string & key)
{
	if (ptr == nullptr)
		return;
	mProgramStates[key] = ptr;
}

/**
*	\brief ����ָ��key����ɫ��״̬
*
*  ���δ�ҵ����򷵻�nullptr
*/
GLProgramStatePtr ResourceCache::GetGLProgramState(const string & key)
{
	auto& it = mProgramStates.find(key);
	if (it != mProgramStates.end())
		return it->second;
	return nullptr;
}

/**
*	\brief ����һ������
*
*   ��ͬ�����ǣ�������Ѿ�������ͬ����������ֱ�ӷ��ض�Ӧ����
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
*	\brief ����ָ��key������
*
*  ���δ�ҵ����򷵻�nullptr
*/
TexturePtr ResourceCache::GetTexture2D(const string & texnname)
{
	auto& it = mTextures.find(texnname);
	if (it != mTextures.end())
		return it->second;
	return nullptr;
}

/**
*	\brief ����һ��ָ����С�Ŀհ�����
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


