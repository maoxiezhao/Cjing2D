#include"core\texture.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"thirdparty\SOIL.h"
#include"thirdparty\stb_image.h"

Texture2D::Texture2D():
	mTextureID(0),
	mWidth(0),
	mHeight(0),
	mInternalFormat(GL_RGBA),
	mImageFormat(GL_RGBA),
	mType(GL_UNSIGNED_BYTE),
	mFilterMax(GL_LINEAR),
	mFilterMin(GL_LINEAR),
	mWrapS(GL_REPEAT),
	mWrapT(GL_REPEAT)
{
}

Texture2D::~Texture2D()
{
}

/**
*	\brief 以image char data初始化texture
*
*	默认不生成mipmap
*/
bool Texture2D::InitWithChars(unsigned char * data)
{
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mImageFormat, mType, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilterMax);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

/**
*	\brief 以image初始化texture
*/
bool Texture2D::InitWithFile(const string & texname)
{
	// 这里以Debug::Error的方式来对文件缺失报错
	if (!FileData::IsFileExists(texname)) 
	{
		Debug::Error("Can not find file:" + texname);
		return false;
	}

	int w, h, n;
	const string data = FileData::ReadFile(texname);
	unsigned char* imageData = stbi_load_from_memory((unsigned char*)data.c_str(), data.length(), &w, &h, &n, mImageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
//	unsigned char* imageData = SOIL_load_image_from_memory((unsigned char*)data.c_str(),data.length(),&w,&h,0, mImageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
//	unsigned char* imageData = SOIL_load_image(texname.c_str(), &w, &h, 0, mImageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);	
	mWidth = (GLuint)w;
	mHeight = (GLuint)h;
	
	bool ret = InitWithChars(imageData);
	stbi_image_free(imageData);
	return ret;
}

/**
*	\brief 绑定为当前使用纹理
*
*	一般而言参与到渲染流水线的纹理，会由Renderer自动绑定
*/
void Texture2D::Bind(GLuint textureActivity) const
{
	if (IsInitialized()) {
		glActiveTexture(textureActivity);		glBindTexture(GL_TEXTURE_2D, mTextureID);
	}
}

GLuint Texture2D::GetTextureID() const
{
	return mTextureID;
}

Size Texture2D::GetSize() const
{
	return Size(mWidth,mHeight);
}

void Texture2D::SetSize(const Size & size)
{
	mWidth = size.width;
	mHeight = size.height;
}

void Texture2D::SetInternalFormat(GLuint format)
{
	mImageFormat = format;
}

void Texture2D::SetImageFormat(GLuint format)
{
	mImageFormat = format;
}

void Texture2D::SetIntenalType(GLuint type)
{
	mType = type;
}

bool Texture2D::IsInitialized() const
{
	return mTextureID != 0;
}

/**
*	\brief 生成mipmap
*/
bool Texture2D::GenerateMipmap(unsigned char * data)
{
	if (!IsInitialized())
		return false;

	// generate....
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 6);
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -1.5f);

//	gluBuild2DMipmaps(GL_TEXTURE_2D, mImageFormat, mWidth, mHeight, mImageFormat, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

RenderTexture::RenderTexture():
	Texture2D(),
	mFrameBuffer(0),
	mDepthRenderBuffer(0),
	mInitialized(false)
{
}

RenderTexture::~RenderTexture()
{
}

/**
*	\brief 重写方法避免调用
*/
bool RenderTexture::InitWithChars(unsigned char * data)
{
	Debug::Warning("Render Texture can only InitWithSize.");
	return false;
}

/**
*	\brief 重写方法避免调用
*/
bool RenderTexture::InitWithFile(const string & texname)
{
	Debug::Warning("Render Texture can only InitWithSize.");
	return false;
}

bool RenderTexture::InitWithSize(int32_t w, int32_t h, bool depthTest)
{
	if (mInitialized)
		return false;

	Debug::CheckAssertion(mFrameBuffer == 0, "The texture frameBuffer already assign.");
	Debug::CheckAssertion(mDepthRenderBuffer == 0, "The texture renderbuffer is already init.");

	glGenFramebuffers(1, &mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

	// 绑定颜色纹理
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, mImageFormat, w, h, 0, mImageFormat, mType, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);

	// 是否添加深度缓冲
	if (depthTest)
	{
		glGenRenderbuffers(1, &mDepthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::Error("[Render] Failed to gen frame buffer");
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mInitialized = true;
	return true;
}

void RenderTexture::BeginDraw()
{
	// if(!mInitialized) return
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTexture::EndDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
