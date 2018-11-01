#pragma once

#include "entity\entityState.h"
#include "player\playerState.h"

/**
*	\brief 推动状态组件
*/
class HurtState : public PlayerState
{
public:
	HurtState(Entity& entity);
	virtual void Start(EntityState& state);
};