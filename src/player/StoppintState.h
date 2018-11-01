#pragma once

#include "entity\entityState.h"
#include "player\playerState.h"

/**
*	\brief 推动状态组件
*/
class StoppingState : public PlayerState
{
public:
	StoppingState(Entity& entity);
	virtual void Start(EntityState& state);
};