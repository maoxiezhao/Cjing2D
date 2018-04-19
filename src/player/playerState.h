#pragma once

#include"entity\entityState.h"

class Player;
class PlayerSprite;
class Equipment;

/**
*	\brief �ƶ�״̬���
*
*	movementState������movment,�����ݵ�ǰ״̬������Ӧ�����߻��߱��ܶ���
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