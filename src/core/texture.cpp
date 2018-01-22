#include"core\texture.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"thirdparty\SOIL.h"

Texture2D::Texture2D():
	mTextureID(0),
	mWidth(0),
	mHeight(0),
	mInternalFormat(GL_RGBA),
	mImageFormat(GL_RGBA),
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
*	\brief ��image char data��ʼ��texture
*
*	Ĭ�ϲ�����mipmap
*/
bool Texture2D::InitWithChars(unsigned char * data)
{
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, mImageFormat, mWidth, mHeight, 0, mImageFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilterMax);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

/**
*	\brief ��image��ʼ��texture
*/
bool Texture2D::InitWithFile(const string & texname)
{
	// ������Debug::Error�ķ�ʽ�����ļ�ȱʧ����
	if (!FileData::IsFileExists(texname)) 
	{
		Debug::Error("Can not find file:" + texname);
		return false;
	}

	int w, h;
	const string data = FileData::ReadFile(texname);
//	unsigned char* imageData = SOIL_load_image_from_memory((unsigned char*)data.c_str(),data.length(),&w,&h,0, mImageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
//	unsigned char* imageData = SOIL_load_image(texname.c_str(), &w, &h, 0, mImageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);	
	mWidth = (GLuint)w;
	mHeight = (GLuint)h;
	
//	return InitWithChars(imageData);
	return false;
}

/**
*	\brief ��Ϊ��ǰʹ������
*
*	һ����Բ��뵽��Ⱦ��ˮ�ߵ���������Renderer�Զ���
*/
void Texture2D::Bind() const
{
	if (IsInitialized())
		glBindTexture(GL_TEXTURE_2D, mTextureID);
	
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

bool Texture2D::IsInitialized() const
{
	return mTextureID != 0;
}

/**
*	\brief ����mipmap
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
