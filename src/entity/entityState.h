#pragma once

#include"entity\entity.h"
#include"game\gameCommands.h"

/**
*	\brief entity 状态组件的基类
*
*	entity state提供了相对于entity本身额外的行为,entity state提供了
*	对entity本身状态的操作能力，但一个entity本身已经提供了完整的行为
*	可以不设置entity state
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