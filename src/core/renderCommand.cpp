#include "renderCommand.h"

RenderCommand::RenderCommand():
	mType(COMMAND_UNKNOW),
	mGlobalOrder(0)
{
}

RenderCommand::RenderCommand(CommandType type, float globalorder):
	mType(type),
	mGlobalOrder(globalorder)
{
}

RenderCommand::~RenderCommand()
{
}

float RenderCommand::GetGlobalOrder() const
{
	return mGlobalOrder;
}

RenderCommand::CommandType RenderCommand::GetCommandType() const
{
	return mType;
}
