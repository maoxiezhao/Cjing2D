#include "quadCommand.h"
#include "core\debug.h"
#include "utils\xxhash.h"
#include "core\renderer.h"

QuadCommand::QuadCommand():
	RenderCommand(COMMAND_QUAD,0.0f),
	mTextureID(0),
	mQuadCounts(0),
	mQuads(nullptr),
	mShadeState(0)
{
}

QuadCommand::~QuadCommand()
{
}

/**
*	\brief 初始化quadCommand
*/
void QuadCommand::Init(float globalOrder, GLProgramStatePtr programState, GLuint textureID, Quad * quads, int quadCounts, 
									const BlendFunc & blendFunc, const Matrix4& transfomr, const Matrix4& modelView)
{
	Debug::CheckAssertion(programState != nullptr, "Invalid programState in QuadCommand::Init().");

	mGlobalOrder = globalOrder;
	mProgramState = programState;
	mQuadCounts = quadCounts;
	mQuads = quads;
	mTransform = transfomr;
	mModelView = modelView;

	if (mTextureID != textureID || mBlendFunc != blendFunc)
	{
		mTextureID = textureID;
		mBlendFunc = blendFunc;
		GenerateShadeState();
	}
}

/**
*	\brief 使用当前着色状态进行渲染
*/
void QuadCommand::UseShade()
{
	Debug::CheckAssertion(mProgramState != nullptr, "Invalid program");

	// 设定混合模式
	glBlendFunc(mBlendFunc.srcBlend, mBlendFunc.dstBlend);

	// 使用着色器
	mProgramState->Apply();
	// 绑定纹理
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
//	mProgramState->SetTexture("Texture0", mTextureID);
	mProgramState->SetUniformMatrix4("projection", mModelView);			// 这里设计存在问题
}

int QuadCommand::GetQuadCounts() const
{
	return mQuadCounts;
}

const Quad* QuadCommand::GetQuads() const
{
	return mQuads;
}

uint32_t QuadCommand::GetShadeState() const
{
	return mShadeState;
}

BlendFunc QuadCommand::GetBlendFunc() const
{
	return mBlendFunc;
}

Matrix4 QuadCommand::GetTransfomr() const
{
	return mTransform;
}

Matrix4 QuadCommand::GetModelView() const
{
	return mModelView;
}

/**
*	\brief 生成当前着色状态
*
*    对纹理Id，混合模式，着色器通过hash函数生成32位无符号整数
*/
void QuadCommand::GenerateShadeState()
{
	int program = mProgramState->GetProgram();
	int hashArray[4] = { program,(int)mTextureID,(int)mBlendFunc.srcBlend ,(int)mBlendFunc.dstBlend };

	mShadeState = XXH32((const void*)hashArray, sizeof(hashArray), 0);
}
