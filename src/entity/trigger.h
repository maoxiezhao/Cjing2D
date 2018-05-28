#pragma once

#include"entity\entity.h"
#include"lua\luaRef.h"

#include<functional>

/**
*	\brief Trigger触发器对象基类
*/
class Trigger : public Entity
{
public:
	enum TriggerMode
	{
		TRIGGER_COLLISION_NONE = 0x00,      // 默认使用RECT检测碰撞
		TRIGGER_COLLISION_CIRCLE = 0x01,	// 圆形的碰撞检测
		TRIGGER_COLLISION_POLYGON = 0x02,   // 多边形的碰撞检测
	};

	Trigger(const std::string& name, int layer, const Point2& pos, TriggerMode triggerMode);

	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;
	TriggerMode GetTriggerType()const;
	
	void SetTriggerTimes(int times);
private:
	bool CanNotifyTrigger(Entity& otherEntity);
	void NotifyTrigger(Entity& otherEntity);

private:
	TriggerMode mTriggerMode;
	int mTriggerTimes;			/** 触发器触发的次数，当为-1时可以无限触发 */
};

/**
*	\brief 矩形Trigger
*/
class RectTrigger : public Trigger
{
public:
	RectTrigger(const std::string& name, int layer, const Point2& pos, const Size& size);
};

