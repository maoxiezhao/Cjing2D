#pragma once

#include "entity\entity.h"

/**
*	\brief Chest类是一种宝箱类，打开是会创建一个或者多个Pickable
*/
class Chest : public Entity
{
public:
	Chest(const std::string& name, const std::string& templName, int layer, 
		const Point2& pos, const std::string& spriteName);

	/** status */
	bool CanOpen();
	bool IsOpen()const;
	void SetItems(const std::string& items);
	std::string GetItmes()const;

	/** notify */
	virtual bool IsObstacle(Entity& entity)const;
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual bool NotifyCommandInteractPressed(Entity& interactEntity);
	virtual void NotifyBeforeCreated();
	virtual void NotifyAfterCreated();

	/** lua */
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

private:
	void SetOpen(bool opened);

private:
	std::string mItems;		/** 宝箱的物品，一系列itemID以_分隔 */
	bool mIsOpen;

	std::string mSpriteName;
	const std::string animateSetID = "entities/common";
};