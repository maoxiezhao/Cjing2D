#pragma once

#include "entity\entityState.h"
#include "player\playerState.h"

/**
*	\brief �ƶ�״̬���
*/
class HurtState : public PlayerState
{
public:
	HurtState(Entity& entity);
	virtual void Start(EntityState& state);
};