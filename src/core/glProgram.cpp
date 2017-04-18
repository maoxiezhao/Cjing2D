#include "glProgram.h"

const string DEFAULT_SPRITE_NORMAL_PROGRAM_NAME = "NormalSprite";
const string DEFAULT_FONT_NORMAL_PROGRAM_NAME = "NormalFont";

GLProgram::GLProgram():
	mProgram(0),
	mVertShader(0),
	mFragShader(0)
{
}

GLProgram::~GLProgram()
{
}

bool GLProgram::InitWithChars(const GLchar * vsChars, const GLchar * fsChars)
{
	return false;
}

bool GLProgram::InitWithFileNames(const string & vsname, const string & fsname)
{
	return false;
}
