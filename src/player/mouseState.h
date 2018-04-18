#pragma once

#include"movementState.h"

/**
*	\brief 鼠标状态
*	
*	鼠标的motion会决定entity的方向
*/
class MouseState : public MovementState
{
public:
	MouseState(Entity& entity);

	virtual void NotifyCommandInteractPressed();	

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();

	virtual float GetFacingDegree()const;
	virtual bool CanAttack()const;
private:
	float mDegree;
};