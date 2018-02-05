#ifndef _SHADE_RESOURCE_H_
#define _SHADE_RESOURCE_H_

#include"common\common.h"
#include"core\glProgram.h"
#include"core\glProgramState.h"
#include"core\texture.h"
#include"core\font.h"

#include<map>
#include<unordered_map>

/**
*	\brief �Ե���ģʽʵ����Ⱦ����Դ�Ĵ洢
*	Ŀǰ֧����Դ��
*		program
*		programState
*		texture
*		font
*/
class ResourceCache
{
public:
	ResourceCache();
	~ResourceCache();

	static ResourceCache& GetInstance();
	void Clear();

	// program
	void LoadDefaultProgram();
	void AddGLProgram(GLProgramPtr ptr, const string& key);
	GLProgramPtr GetGLProgram(const string& key);

	// programState
	void LoadDefaultProgramState();
	void AddGLProgramState(GLProgramStatePtr ptr, const string& key);
	GLProgramStatePtr GetGLProgramState(const string& key);

	// texture
	//TexturePtr LoadTextureAllTime(const string& texname);
	TexturePtr LoadTexture2D(const string& texname);
	TexturePtr GetTexture2D(const string& texnname);
	TexturePtr CreateTexture2DBySize(const string& texname, const Size& size);
	RenderTexturePtr CreateRenderTexture2DBySize(const string& name, const Size& size);

	// font

private:
	std::unordered_map<std::string, GLProgramPtr> mPrograms;
	std::unordered_map<std::string, GLProgramStatePtr> mProgramStates;
	std::unordered_map<std::string, TexturePtr> mTextures;
	std::unordered_map<std::string, font::FontPtr> mFonts;
};


#endif