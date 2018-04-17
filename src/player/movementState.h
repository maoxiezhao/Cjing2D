#pragma once

#include"entity\entityState.h"
#include"movements\movement.h"
#include "movements\playerMovement.h"

/**
*	\brief �ƶ�״̬���
*
*	movementState������movment,�����ݵ�ǰ״̬������Ӧ�����߻��߱��ܶ���
*/
class MovementState : public EntityState
{
public:
	MovementState(Entity& entity);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();

	virtual Direction8 GetWantedDirection8()const;
	virtual void NotifyMovementChanged();
	virtual void NotifyCommandShiftPressed();

	virtual void SetPlayerWalkingAnimation();
	virtual void SetPlayerStopAnimation();
	virtual void SetPlayerShiftingAnimation();
private:
	std::shared_ptr<PlayerMovement> mPlayerMovement;

};