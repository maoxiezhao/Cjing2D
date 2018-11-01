#pragma once

#include"common\common.h"
#include"core\types.h"
#include"core\vertexArray.h"
#include"core\glProgramState.h"
#include"core\light.h"
#include"utils\matrix4.h"

/**
*	\brief RenderPass渲染阶段
*
*	渲染阶段带有渲染绘制步骤,一个RenderPass
*   的处理分为3个步骤
*/
class RenderPass
{
public:
	RenderPass();
	~RenderPass();

	virtual bool Initialize();
	virtual bool Quit();

	virtual void PrePrecoss();
	virtual void Processing();
	virtual void PostProcess();
};