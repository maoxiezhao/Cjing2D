#ifndef _RENDER_COMMAND_H_
#define _RENDER_COMMAND_H_

#include"common\common.h"

/**
*	\brief 渲染命令的基类，提供命令的类型和全局Z坐标值
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