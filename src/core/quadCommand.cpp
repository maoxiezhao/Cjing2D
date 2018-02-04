#include "quadCommand.h"
#include "core\debug.h"
#include "utils\xxhash.h"
#include "core\renderer.h"

QuadCommand::QuadCommand():
	RenderCommand(COMMAND_QUAD,0.0f),
	mTextureID(0),
	mQuadCounts(0),
	mQuads(),
	mShadeState(0),
	mIsDeferredShade(false)
{
}

QuadCommand::~QuadCommand()
{
}

/**
*	\brief 初始化quadCommand
*/
void QuadCommand::Init(float globalOrder, GLProgramStatePtr programState, GLuint textureID, GLuint normalTextureID, Quad quads, int quadCounts,
									const BlendFunc & blendFunc, const Matrix4& transfomr, const Matrix4& modelView, bool autoReleased)
{
	Debug::CheckAssertion(programState != nullptr, "Invalid programState in QuadCommand::Init().");

	mGlobalOrder = globalOrder;
	mQuadCounts = quadCounts;
	mQuads = quads;
	mTransform = transfomr;
	mModelView = modelView;
	SetAutoReleased(autoReleased);

	if (mTextureID != textureID || mNormalTextureID != normalTextureID ||mBlendFunc != blendFunc || mProgramState != programState)
	{
		mProgramState = programState;
		mTextureID = textureID;
		mNormalTextureID = normalTextureID;
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
	if (mNormalTextureID != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mNormalTextureID);
	}
//	mProgramState->SetTexture("Texture0", mTextureID);
	mProgramState->SetUniformMatrix4("projection", mModelView);			// 这里设计存在问题
}

int QuadCommand::GetQuadCounts() const
{
	return mQuadCounts;
}

const Quad* QuadCommand::GetQuads() const
{
	return &mQuads;
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

bool QuadCommand::IsDeferredShade() const
{
	return mIsDeferredShade;
}

void QuadCommand::SetDeferredShade(bool isDeferred)
{
	mIsDeferredShade = isDeferred;
}

void QuadCommand::SetNormalTexture(GLuint normalTextureID)
{
}

/**
*	\brief 生成当前着色状态
*
*    对纹理Id，混合模式，着色器通过hash函数生成32位无符号整数
*/
void QuadCommand::GenerateShadeState()
{
	int program = mProgramState->GetProgram();
	int hashArray[6] = { program,static_cast<int>(mProgramState->GetProgramStateID()),
		(int)mTextureID, (int)mNormalTextureID, (int)mBlendFunc.srcBlend ,(int)mBlendFunc.dstBlend };

	mShadeState = XXH32((const void*)hashArray, sizeof(hashArray), 0);
}
