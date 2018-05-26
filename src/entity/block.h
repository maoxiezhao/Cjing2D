#pragma once

#include "entity\entity.h"

/**
*	\brief Block类是一系列可推动、可拉动、可破坏的实体
*/
class Block : public Entity
{
public:
	Block(const std::string& name, int layer, const Point2& pos, 
		const std::string& spriteName, bool canPushed, bool canPulled, bool canDesotry);

	/** notify */
	virtual bool IsObstacle(Entity& entity)const;
	virtual bool StartMoveByPushed(Entity& entity);
	virtual bool StopMoveByPushed();
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual bool NotifyCommandInteractPressed(Entity& interactEntity);
	virtual void NotifyObstacleReached();

	/** status */
	bool IsCanPushed()const;
	void SetCanPushed(bool pushed);
	bool IsCanPulled()const;
	void SetCanPulled(bool pulled);
	bool IsCanDestroy()const;
	void SetCanDestroy(bool destroy);

	/** lua */
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

private:
	uint32_t mMovingDelay;
	uint32_t mCanMoveDate;

	bool mCanPushed;
	bool mCanPulled;
	bool mCanDestory;

	EntityPtr mMovingEntity;

	const std::string animateSetID = "entities/common";
};