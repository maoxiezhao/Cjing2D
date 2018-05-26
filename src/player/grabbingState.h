#pragma once

#include "entity\entityState.h"
#include "player\playerState.h"

/**
*	\brief ×´Ì¬×é¼þ
*/
class GrabbingState : public PlayerState
{
public:
	GrabbingState(Entity& entity);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();
	virtual bool IsCanHurt()const;

};