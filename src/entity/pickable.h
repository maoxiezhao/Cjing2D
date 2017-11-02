#pragma once

#include"entity\entity.h"
#include"game\itemAcquired.h"
#include"movements\sequenceMovement.h"

/**
*	\brief 可拾取对象
*
*	可拾取对象仅仅是触发拾取作用的entity
*	具体拾取操作由tresure
*/
class Pickable : public Entity
{
public:
	Pickable(const std::string itemName, int layer, const Point2& pos, ItemAcquired& itemAcquired);

	/** system */
	virtual void Update();
	virtual void Draw();
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

	/** notify */
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual bool NotifyCommandInteractPressed(Entity& interactEntity);

private:
	bool InitSprites();
	void TryGiveItemToPicker(Entity& picker);

	bool mCanPicked;	

	ItemAcquired mItemAcquired;	/** 当前获取物品的描述对象 */
	AnimationSpritePtr mItemSprite;
	AnimationSpritePtr mShadowSprite;
	std::shared_ptr<SequenceMovement> mMovemment;
	
};