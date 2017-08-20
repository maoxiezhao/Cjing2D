#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"common\common.h"
#include"utils\size.h"

/**
*	\brief 2dŒ∆¿Ì
*/
class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	bool InitWithChars(unsigned char* data);
	bool InitWithFile(const string& texname);
	void Bind()const;

	GLuint GetTextureID()const;
	Size GetSize()const;
	void SetSize(const Size& size);
	void SetInternalFormat(GLuint format);
	void SetImageFormat(GLuint format);

	bool IsInitialized()const;
	bool GenerateMipmap(unsigned char * data);

private:
	GLuint mTextureID;
	GLuint mWidth;
	GLuint mHeight;
	GLuint mInternalFormat;
	GLuint mImageFormat;
	GLuint mWrapS;
	GLuint mWrapT;
	GLuint mFilterMin;
	GLuint mFilterMax;

};

using TexturePtr = std::shared_ptr<Texture2D>;

#endif