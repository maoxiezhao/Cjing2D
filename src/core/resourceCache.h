#ifndef _SHADE_RESOURCE_H_
#define _SHADE_RESOURCE_H_

#include"common\common.h"
#include"core\glProgram.h"
#include"core\glProgramState.h"
#include"core\texture.h"
#include<map>
#include<unordered_map>

/**
*	\brief 以单例模式实现渲染类资源的存储
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

private:
	std::unordered_map<std::string, GLProgramPtr> mPrograms;
	std::unordered_map<std::string, GLProgramStatePtr> mProgramStates;
	std::unordered_map<std::string, TexturePtr> mTextures;

};


#endif