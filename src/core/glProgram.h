#ifndef _GL_PROGRAM_H_
#define _GL_PROGRAM_H_

#include"common\common.h"

/**
*	\brief ×ÅÉ«Æ÷³ÌÐò
*/
class GLProgram
{
public:
	GLProgram();
	virtual ~GLProgram();

	static const string DEFAULT_SPRITE_NORMAL_PROGRAM_NAME;
	static const string DEFAULT_FONT_NORMAL_PROGRAM_NAME;

	bool InitWithChars(const GLchar* vsChars, const GLchar* fsChars);
	bool InitWithFileNames(const string& vsname,const string& fsname);

private:
	GLuint mProgram;
	GLuint mVertShader;
	GLuint mFragShader;

};

using GLProgramPtr = std::shared_ptr<GLProgram>;

#endif