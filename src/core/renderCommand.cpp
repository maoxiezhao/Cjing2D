#include "renderCommand.h"

RenderCommand::RenderCommand():
	mType(COMMAND_UNKNOW),
	mGlobalOrder(0),
	mIsAutoReleased(false)
{
}

RenderCommand::RenderCommand(CommandType type, float globalorder):
	mType(type),
	mGlobalOrder(globalorder),
	mIsAutoReleased(false)
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

void RenderCommand::SetAutoReleased(bool autoReleased)
{
	mIsAutoReleased = autoReleased;
}

bool RenderCommand::IsAutoReleased() const
{
	return mIsAutoReleased;
}
