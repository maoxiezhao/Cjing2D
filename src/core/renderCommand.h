#ifndef _RENDER_COMMAND_H_
#define _RENDER_COMMAND_H_

#include"common\common.h"

/**
*	\brief ��Ⱦ����Ļ��࣬�ṩ��������ͺ�ȫ��Z����ֵ
*/
class RenderCommand
{
public:
	enum CommandType
	{
		COMMAND_QUAD,
		COMMAND_BATCH,
		COMMAND_CUSTOMER,
		COMMAND_UNKNOW
	};
	RenderCommand();
	RenderCommand(CommandType type, float globalorder = 0);
	virtual~RenderCommand();

	float GetGlobalOrder()const;
	CommandType GetCommandType()const;

	void SetAutoReleased(bool autoReleased);
	bool IsAutoReleased()const;

protected:
	CommandType mType;
	float mGlobalOrder;
	bool mIsAutoReleased;

};

#endif