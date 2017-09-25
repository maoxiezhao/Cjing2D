#pragma once

#include"movementState.h"

/**
*	\brief 自由状态,该状态是player默认使用的状态
*/
class FreeState : public MovementState
{
public:
	FreeState(Entity& entity);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();

	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);

	virtual void SetPlayerWalkingAnimation();
	virtual void SetPlayerStopAnimation();

private:

};