#pragma once

#include"common\common.h"
#include"core\types.h"
#include"core\vertexArray.h"
#include"core\glProgramState.h"
#include"core\light.h"
#include"utils\matrix4.h"

/**
*	\brief RenderPass��Ⱦ�׶�
*
*	��Ⱦ�׶δ�����Ⱦ���Ʋ���,һ��RenderPass
*   �Ĵ����Ϊ3������
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