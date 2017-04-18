#include "quadCommand.h"

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
void QuadCommand::Init(float globalOrder, GLuint textureID, Quad * quads, int quadCounts, const BlendFunc & blendFunc)
{
	if(mTextureID != textureID || mBlendFunc != blendFunc)
		GenerateShadeState();
}

/**
*	\brief 使用当前着色状态进行渲染
*/
void QuadCommand::UseShade()
{
	// 使用着色器

	// 绑定纹理

	// 设定混合模式
	glBlendFunc(mBlendFunc.srcBlend, mBlendFunc.dstBlend);
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

/**
*	\brief 生成当前着色状态
*
*    对纹理Id，混合模式，着色器通过hash函数生成32位无符号整数
*/
void QuadCommand::GenerateShadeState()
{
	mShadeState = 0;
}
