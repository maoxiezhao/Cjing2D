#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"common\common.h"
#include"utils\size.h"

/**
*	\brief 2d纹理
*/
class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	bool InitWithChars(unsigned char* data);
	bool InitWithFile(const string& texname);
	void Bind(GLuint textureActivity = GL_TEXTURE0)const;

	GLuint GetTextureID()const;
	Size GetSize()const;
	void SetSize(const Size& size);
	void SetInternalFormat(GLuint format);
	void SetImageFormat(GLuint format);
	void SetIntenalType(GLuint type);

	bool IsInitialized()const;
	bool GenerateMipmap(unsigned char * data);

protected:
	GLuint mTextureID;
	GLuint mWidth;
	GLuint mHeight;
	GLuint mInternalFormat;
	GLuint mImageFormat;
	GLuint mType;
	GLuint mWrapS;
	GLuint mWrapT;
	GLuint mFilterMin;
	GLuint mFilterMax;

};
using TexturePtr = std::shared_ptr<Texture2D>;

/**
*	\brief 可渲染至纹理的纹理
*
*	使用帧缓冲方式将当前渲染绘制到texture中
*	因为会占用大量的GPU带宽，不推荐频繁使用
*/
class RenderTexture : public Texture2D
{
public:
	RenderTexture();
	~RenderTexture();

	bool InitWithChars(unsigned char* data);
	bool InitWithFile(const string& texname);
	bool InitWithSize(int32_t w, int32_t h, bool depthTest = false);
	void BeginDraw();
	void EndDraw();

private:
	GLuint mFrameBuffer;
	GLuint mDepthRenderBuffer;
	bool mInitialized;
};
using RenderTexturePtr = std::shared_ptr<RenderTexture>;

#endif