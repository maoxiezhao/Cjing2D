#pragma once

#include "entity\entityState.h"
#include "player\playerState.h"

class PathMovement;

/**
*	\brief 推动状态组件
*/
class PushingState : public PlayerState
{
public:
	PushingState(Entity& entity);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();
	virtual void NotifyMovementFinished();
	virtual void NotifyStopGrabbing();
	virtual Direction4 GetWantedDirection4()const;

private:
	bool IsPushingEntity()const;
	void StopPushingEntity();
	void PlayerPositionAdjust();

private:
	Direction4 mPushingDirection;
	Entity* mPushingEntity;
	std::shared_ptr<PathMovement> mMovement;

};