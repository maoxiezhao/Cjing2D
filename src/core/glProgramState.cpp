#include "glProgramState.h"
#include "debug.h"

const string GLProgramState::DEFAULT_SPRITE_NORMAL_PROGRAMSTATE_NAME = "normal_sprite_state";
const string GLProgramState::DEFAULT_SPRITE_COLOR_PROGRAMSTATE_NAME = "color_sprite_state";
const string GLProgramState::DEFAULT_FONT_NORMAL_PROGRAMSTATE_NAME = "normal_text_state";

uint32_t GLProgramState::ProgramStateID = 0;

UniformValue::UniformValue() :
	mUniform(nullptr)
{
}

UniformValue::UniformValue(Uniform* uniform, GLProgram* program):
	mUniform(uniform),
	mProgram(program)
{
}

UniformValue::~UniformValue()
{
}

/**
*	\brief 应用当前uniformvalue
*/
void UniformValue::Apply()
{
	switch (mUniform->type)
	{
	case GL_INT:
		mProgram->SetUniform1i(mUniform->location, mValue.intValue);
		break;
	case GL_INT_VEC3:
		mProgram->SetUniform3i(mUniform->location, mValue.intArray[0], mValue.intArray[1], mValue.intArray[2]);
		break;
	case GL_FLOAT:
		mProgram->SetUniform1f(mUniform->location, mValue.flaotValue);
		break;
	case GL_FLOAT_VEC3:
		mProgram->SetUniform3f(mUniform->location, mValue.floatArray[0], mValue.floatArray[1], mValue.floatArray[2]);
		break;
	case GL_FLOAT_MAT4:
		mProgram->SetUniformMatrix4f(mUniform->location, mValue.matrixValue);
		break;
	case GL_SAMPLER_2D:
		mProgram->SetUniform1i(mUniform->location, mValue.mTextureId);
		glBindTexture(GL_TEXTURE_2D, mValue.mTextureId);
		break;
	default:
		Debug::Error("Invalid Uniform Value.");
		break;
	}
}

void UniformValue::SetInteger(int value)
{
	Debug::CheckAssertion(mUniform->type == GL_INT, "Invalid SetUniform operator.");
	mValue.intValue = value;
}

void UniformValue::SetFloat(float value)
{
	Debug::CheckAssertion(mUniform->type == GL_FLOAT, "Invalid SetUniform operator.");
	mValue.flaotValue = value;
}

void UniformValue::SetVec3i(const Vec3i & vec3i)
{
	Debug::CheckAssertion(mUniform->type == GL_INT_VEC3, "Invalid SetUniform operator.");
	mValue.intArray[0] = vec3i.x;
	mValue.intArray[1] = vec3i.y;
	mValue.intArray[2] = vec3i.z;
}

void UniformValue::SetVec3f(const Vec3f & vec3f)
{
	Debug::CheckAssertion(mUniform->type == GL_FLOAT_VEC3, "Invalid SetUniform operator.");
	mValue.floatArray[0] = vec3f.x;
	mValue.floatArray[1] = vec3f.y;
	mValue.floatArray[2] = vec3f.z;
}

void UniformValue::SetMat4(const Matrix4 & mat)
{
	Debug::CheckAssertion(mUniform->type == GL_FLOAT_MAT4, "Invalid SetUniform operator.");
	memcpy(mValue.matrixValue, &mat, sizeof(mValue.matrixValue));
}

void UniformValue::SetTexture(GLuint textureId)
{
	Debug::CheckAssertion(mUniform->type == GL_SAMPLER_2D, "Invalid SetUniform operator.");
	mValue.mTextureId = textureId;
}

void UniformValue::SetUniform(Uniform* uniform)
{
	mUniform = uniform;
}


////////////////////////////////////////////////////////////////////////////////////

GLProgramState::GLProgramState():
	mProgram(nullptr),
	mUniformValueDirty(true),
	mProgramStateID(GLProgramState::ProgramStateID++)
{
}

GLProgramState::~GLProgramState()
{
}

/**
*	\brief 应用当前着色器状态
*/
void GLProgramState::Apply()
{
	ApplyProgram();
	ApplyAttibutes();
	ApplyUniforms();
}

/**
*	\brief 应用当前着色器
*
*    如果需要获取当前uniforms信息，则将需要设置的uniform保存在mUniforms
*/
void GLProgramState::ApplyProgram()
{
	Debug::CheckAssertion(mProgram != nullptr, "Invaild program.");
	if (mUniformValueDirty)
	{	// 如果需要
		for (auto& uniform : mUniformNames)
			mUniforms[uniform.second].SetUniform(mProgram->GetUniform(uniform.first));

		mUniformValueDirty = true;
	}
	mProgram->Use();
}

/**
*	\brief 应用每个attrubute
*/
void GLProgramState::ApplyAttibutes()
{
}

/**
*	\brief 应用每个uniform
*/
void GLProgramState::ApplyUniforms()
{
	for (auto& unifomr : mUniforms)
		unifomr.second.Apply();
}

uint32_t GLProgramState::GetProgramStateID() const
{
	return mProgramStateID;
}

UniformValue* GLProgramState::GetUniform(const string & key) 
{
	auto& it = mUniformNames.find(key);
	if (it != mUniformNames.end())
		return &mUniforms[it->second];
}

UniformValue* GLProgramState::GetUniform(const GLint & key) 
{
	auto& it = mUniforms.find(key);
	if (it != mUniforms.end())
		return &it->second;
}

/**
*	\brief 设置GLProgram
*/
void GLProgramState::Set(GLProgramPtr program)
{
	Debug::CheckAssertion(program != nullptr, "Invaild program.");
	if (program != mProgram)
	{
		ResetProgram();
		Init(program);
	}
}

GLuint GLProgramState::GetProgram() const
{
	if (mProgram == nullptr)
	{
		Debug::Warning("Empty Program.");
		return -1;
	}
	else
		return mProgram->GetProgram();
}

void GLProgramState::SetTexture(const string & name, GLuint textureId)
{
	UniformValue* uniform = GetUniform(name);
	if (uniform)
		uniform->SetTexture(textureId);
	else
		Debug::Error("Can not find uniform value named by " + name);
}

void GLProgramState::SetUniform1i(const string & name, int i1)
{
	UniformValue* uniform = GetUniform(name);
	if (uniform)
		uniform->SetInteger(i1);
	else
		Debug::Error("Can not find uniform value named by " + name);
}

void GLProgramState::SetUniform3i(const string & name, const Vec3i& vec3i)
{
	UniformValue* uniform = GetUniform(name);
	if (uniform)
		uniform->SetVec3i(vec3i);
	else
		Debug::Error("Can not find uniform value named by " + name);
}

void GLProgramState::SetUniform1f(const string & name, float f1)
{
	UniformValue* uniform = GetUniform(name);
	if (uniform)
		uniform->SetFloat(f1);
	else
		Debug::Error("Can not find uniform value named by " + name);
}

void GLProgramState::SetUniform3f(const string & name, const Vec3f& vec3f)
{
	UniformValue* uniform = GetUniform(name);
	if (uniform)
		uniform->SetVec3f(vec3f);
	else
		Debug::Error("Can not find uniform value named by " + name);
}


void GLProgramState::SetUniformMatrix4(const string & name, const Matrix4 & mat)
{
	UniformValue* uniform = GetUniform(name);
	if (uniform)
		uniform->SetMat4(mat);
	else
		Debug::Error("Can not find uniform value named by " + name);
}

/**
*	\brief 初始化ProgramState
*
*   从program获取uniforms保存
*/
void GLProgramState::Init(GLProgramPtr program)
{
	Debug::CheckAssertion(program != nullptr, "Invaild program.");
	mProgram = program;

	for (auto& uniform : program->GetUniforms())
	{
		UniformValue newUniformValue(&uniform.second,program.get());
		mUniformNames[uniform.first] = uniform.second.location;
		mUniforms[uniform.second.location] = newUniformValue;
	}
}

void GLProgramState::ResetProgram()
{
	mUniformValueDirty = true;
}

