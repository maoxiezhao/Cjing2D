#include"core\glProgram.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"core\logger.h"

const string GLProgram::DEFAULT_SPRITE_NORMAL_PROGRAM_NAME = "normal_sprite";
const string GLProgram::DEFAULT_SPRITE_COLOR_PROGRAM_NAME = "color_sprite";
const string GLProgram::DEFAULT_G_BUFFER_PROGRAM_NAME = "gBuffer";
const string GLProgram::DEFAULT_DEFERRED_LIGHT_PROGRAM_NAME = "deferred_shade";
const string GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME = "outlined_sprite";
const string GLProgram::DEFAULT_FONT_NORMAL_PROGRAM_NAME = "normal_text";

GLProgram::GLProgram():
	mProgram(0),
	mVertShader(0),
	mFragShader(0)
{
}

GLProgram::~GLProgram()
{
	mUniforms.clear();
	if (mVertShader)
		glDeleteShader(mVertShader);
	if (mFragShader)
		glDeleteShader(mFragShader);
	if (mProgram)
		glDeleteProgram(mProgram);
	mVertShader = mFragShader = mProgram = 0;
}

/**
*	\brief ���ַ������г�ʼ��GLprogram
*/
bool GLProgram::InitWithChars(const GLchar * vsChars, const GLchar * fsChars)
{
	mProgram = glCreateProgram();
	mVertShader = mFragShader = 0;
	// compile
	if (!CompileShader(&mVertShader, GL_VERTEX_SHADER, vsChars))
	{
		Debug::Error("Vertex shader compile-time error.");
		return false;
	}
	if (!CompileShader(&mFragShader, GL_FRAGMENT_SHADER, fsChars))
	{
		Debug::Error("Fragment shader compile-time error.");
		return false;
	}
	// attach
	if (mVertShader)
		glAttachShader(mProgram, mVertShader);
	if (mFragShader)
		glAttachShader(mProgram, mFragShader);

	return true;
}

/**
*	\brief ���ļ��г�ʼ��GLProgram
*/
bool GLProgram::InitWithFileNames(const string & vsname, const string & fsname)
{
	Logger::Info(string("[Render] Load GLProgram vsname:'") + vsname + "'  fsname:'" + fsname);
	string vsString = FileData::ReadFile("shaders/" + vsname);
	string fsString = FileData::ReadFile("shaders/" + fsname);
	return InitWithChars(vsString.c_str(), fsString.c_str());
}

/**
*	\brief ����unifomrs
*/
std::unordered_map<string, Uniform>& GLProgram::GetUniforms()
{
	return mUniforms;
}

/**
*	\brief ����uniform
*/
Uniform *GLProgram::GetUniform(const string & name)
{
	auto& it = mUniforms.find(name);
	if (it != mUniforms.end())
		return &it->second;
	return nullptr;
}

GLuint GLProgram::GetProgram() const
{
	return mProgram;
}

/**
*	\brief ����uniform
*/
void GLProgram::UpdateUnifrom()
{
}

/**
*	\brief ������ɫ������
*/
void GLProgram::Link()
{
	Debug::CheckAssertion(mProgram != 0, "Can not link invalid program.");

	glLinkProgram(mProgram);
	ParseUniforms();

	if (mVertShader)
		glDeleteShader(mVertShader);
	if (mFragShader)
		glDeleteShader(mFragShader);
	mVertShader = mFragShader = 0;
}

/**
*	\brief ʹ����ɫ��Ϊ��ǰ��ɫ��
*/
void GLProgram::Use()
{
	Debug::CheckAssertion(mProgram != 0, "Can not use invalid program.");
	glUseProgram(mProgram);
}

/**
*	\brief ����Integer��ָ��λ�õ�uniform
*/
void GLProgram::SetUniform1i(GLint location, GLint i1)
{
	glUniform1i(location, i1);
}

void GLProgram::SetUniform3i(GLint location, GLint i1, GLint i2, GLint i3)
{
	glUniform3i(location, i1, i2, i3);
}

void GLProgram::SetUniform1f(GLint location, GLfloat f1)
{
	glUniform1f(location, f1);
}

void GLProgram::SetUniform3f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3)
{
	glUniform3f(location, f1, f2, f3);
}

void GLProgram::SetUniformMatrix4f(GLint location, const GLfloat * mat)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}

/**
*	\brief ������ɫ���ļ�
*/
bool GLProgram::CompileShader(GLuint * shader, GLenum type, const GLchar * source)
{
	if (!shader || !source)
		return false;
	GLint status;
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &source, nullptr);
	glCompileShader(*shader);
	
	// get debug info
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
	if (!status)
	{	// have error
		GLchar infoLog[1024];
		glGetShaderInfoLog(*shader, 1024, nullptr, infoLog);
		Debug::Error("vertex shaer compile-time error:" + string(infoLog));
		return false;

	}
	return (status = GL_TRUE);
}

/**
*	\brief ������ɫ���е�uniform
*
*    һ����link֮����ã���ȡuniform����Ϣ��������mUniform��һ������
*	 ��programState�д���uniform
*/
void GLProgram::ParseUniforms()
{
	mUniforms.clear();
	GLint activeUniforms;
	glGetProgramiv(mProgram, GL_ACTIVE_UNIFORMS, &activeUniforms);
	if (activeUniforms > 0)
	{
		GLint maxLength;
		glGetProgramiv(mProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
		if (maxLength > 0)
		{
			GLchar* uniformName = new GLchar[maxLength + 1];
			Uniform newUniform;
			for (int index = 0; index < activeUniforms; ++index)
			{	// ��ȡ����uniform����Ϣ
				glGetActiveUniform(mProgram, index, maxLength, nullptr, &newUniform.size, &newUniform.type, uniformName);
				uniformName[maxLength] = '\0';
				
				newUniform.name = string(uniformName);
				newUniform.location = glGetUniformLocation(mProgram, uniformName);
				mUniforms[uniformName] = newUniform;
			}
			delete uniformName;
		}
	}
}
