#pragma once

#include"entity\entity.h"
#include"game\gameCommands.h"

/**
*	\brief entity ״̬����Ļ���
*
*	entity state�ṩ�������entity����������Ϊ,entity state�ṩ��
*	��entity����״̬�Ĳ�����������һ��entity�����Ѿ��ṩ����������Ϊ
*	���Բ�����entity state
*/

class EntityState 
{
public:
	EntityState(Entity& entity);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();

	/** command notify */
	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);
	virtual void NotifyMovementChanged();
	virtual Direction8 GetWantedDirection8()const;

	Entity& GetEntity();

private:
	Entity& mEntity;

};