#pragma once

#include"entity\entity.h"
#include"game\gameCommands.h"

class Game;

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
	EntityState(Entity& entity, const std::string& name);

	virtual void Start(EntityState& state);
	virtual void Stop(EntityState& state);
	virtual void Update();
	virtual void SetSuspended(bool suspended);
	bool IsSuspended()const;

	/** command notify */
	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);
	virtual void NotifyMovementChanged();
	virtual void NotifyMovementFinished();

	// notify special command
	virtual void NotifyCommandDirectionPressed(Direction4 direction);
	virtual void NotifyCommandDirectionReleased(Direction4 direction);
	virtual void NotifyCommandInteractPressed();
	virtual void NotifyCommandShiftPressed();
	virtual void NotifyCommandAttackPressed();
	virtual void NotifyStopGrabbing();

	std::string GetName()const;
	virtual Direction8 GetWantedDirection8()const;
	virtual Direction4 GetWantedDirection4()const;
	virtual bool CanAttack()const;
	virtual float GetFacingDegree()const;

	Game& GetGame();
	const Game& GetGame()const;
	Entity& GetEntity();
	const Entity& GetEntity()const;

private:
	Entity& mEntity;
	std::string mName;
	bool mIsSuspended;
};