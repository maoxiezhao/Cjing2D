#pragma once

#include "entity\entityState.h"
#include "movements\movement.h"
#include "movements\playerMovement.h"
#include "player\playerState.h"

/**
*	\brief 移动状态组件
*
*	movementState会设置movment,并根据当前状态设置相应的行走或者奔跑动画
*/
class MovementState : public PlayerState
{
public:
	MovementState(Entity& entity, const std::string& name);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();

	virtual Direction4 GetWantedDirection4()const;
	virtual Direction8 GetWantedDirection8()const;
	virtual void NotifyMovementChanged();
	virtual void NotifyCommandShiftPressed();

	virtual void SetPlayerWalkingAnimation();
	virtual void SetPlayerStopAnimation();
	virtual void SetPlayerShiftingAnimation();
private:
	std::shared_ptr<PlayerMovement> mPlayerMovement;
	Direction4 mDirection;
};