#ifndef _GL_PROGRAM_H_
#define _GL_PROGRAM_H_

#include"common\common.h"
#include"utils\matrix4.h"
#include<unordered_map>

/** 
*	\brief 着色器变量
*/
struct Uniform
{
	string  name;
	GLsizei size;
	GLenum  type;
	GLint   location;
};

/**
*	\brief 着色器程序
*/
class GLProgram
{
public:
	GLProgram();
	virtual ~GLProgram();

	static const string DEFAULT_SPRITE_NORMAL_PROGRAM_NAME;
	static const string DEFAULT_SPRITE_COLOR_PROGRAM_NAME;
	static const string DEFAULT_G_BUFFER_PROGRAM_NAME;
	static const string DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME;
	static const string DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME;
	static const string DEFAULT_FONT_NORMAL_PROGRAM_NAME;

	bool InitWithChars(const GLchar* vsChars, const GLchar* fsChars);
	bool InitWithFileNames(const string& vsname,const string& fsname);

	std::unordered_map<string, Uniform>& GetUniforms();
	Uniform* GetUniform(const string& name);
	void UpdateUnifrom();
	GLuint GetProgram()const;

	void Link();
	void Use();

	// uniform
	void SetUniform1i(GLint location, GLint i1);
	void SetUniform3i(GLint location, GLint i1, GLint i2, GLint i3);
	void SetUniform1f(GLint location, GLfloat f1);
	void SetUniform3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3);
	void SetUniformMatrix4f(GLint location, const GLfloat* mat);

private:
	bool CompileShader(GLuint*shader, GLenum type,const GLchar* source);
	void ParseUniforms();

	GLuint mProgram;
	GLuint mVertShader;
	GLuint mFragShader;

	std::unordered_map<string, Uniform> mUniforms;

};

using GLProgramPtr = std::shared_ptr<GLProgram>;

#endif