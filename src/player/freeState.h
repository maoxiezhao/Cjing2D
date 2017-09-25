#pragma once

#include"movementState.h"

/**
*	\brief ����״̬,��״̬��playerĬ��ʹ�õ�״̬
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