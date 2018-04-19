#pragma once

#include"entity\entityState.h"

class Player;
class PlayerSprite;
class Equipment;

/**
*	\brief 移动状态组件
*
*	movementState会设置movment,并根据当前状态设置相应的行走或者奔跑动画
*/
class PlayerState : public EntityState
{
public:
	PlayerState(Entity& entity);

	virtual void Update();

	virtual Player& GetPlayer();
	virtual const Player& GetPlayer()const;
	virtual PlayerSprite& GetPlayerSprites();
	virtual Equipment& GetEquipment();

	virtual void NotifyCommandAttackPressed();
	virtual void NotifyCommandSwitchPressed();
};