#pragma once

#include "entity\entityState.h"
#include "player\playerState.h"

/**
*	\brief �ƶ�״̬���
*/
class StoppingState : public PlayerState
{
public:
	StoppingState(Entity& entity);
	virtual void Start(EntityState& state);
};