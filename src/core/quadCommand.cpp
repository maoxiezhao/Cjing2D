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
*	\brief ��ʼ��quadCommand
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
*	\brief ʹ�õ�ǰ��ɫ״̬������Ⱦ
*/
void QuadCommand::UseShade()
{
	Debug::CheckAssertion(mProgramState != nullptr, "Invalid program");

	// �趨���ģʽ
	glBlendFunc(mBlendFunc.srcBlend, mBlendFunc.dstBlend);

	// ʹ����ɫ��
	mProgramState->Apply();
	// ������
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	if (mNormalTextureID != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mNormalTextureID);
	}
//	mProgramState->SetTexture("Texture0", mTextureID);
	mProgramState->SetUniformMatrix4("projection", mModelView);			// ������ƴ�������
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
*	\brief ���ɵ�ǰ��ɫ״̬
*
*    ������Id�����ģʽ����ɫ��ͨ��hash��������32λ�޷�������
*/
void QuadCommand::GenerateShadeState()
{
	int program = mProgramState->GetProgram();
	int hashArray[6] = { program,static_cast<int>(mProgramState->GetProgramStateID()),
		(int)mTextureID, (int)mNormalTextureID, (int)mBlendFunc.srcBlend ,(int)mBlendFunc.dstBlend };

	mShadeState = XXH32((const void*)hashArray, sizeof(hashArray), 0);
}
