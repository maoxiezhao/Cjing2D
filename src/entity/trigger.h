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
	enum TriggerCollisionMode
	{
		TRIGGER_COLLISION_NONE = 0x00,      // 默认使用RECT检测碰撞
		TRIGGER_COLLISION_CIRCLE = 0x01,	// 圆形的碰撞检测
		TRIGGER_COLLISION_POLYGON = 0x02,   // 多边形的碰撞检测
	};

	Trigger(const std::string& name, int layer, const Point2& pos, TriggerCollisionMode triggerCollisonMode);
	~Trigger();

	/** system */
	virtual void Update();
	virtual void Draw();
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

	/** notify */
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual bool NotifyCommandInteractPressed(Entity& interactEntity);

	void SetTriggerCallBack(const std::function<void()>& callback);
	void SetTriggerCallBack(const LuaRef& callback);

private:
	TriggerCollisionMode mTriggerCollisonMode;
	std::function<void()> mFinishCallBack;		/** 结束回调函数 */

};

/**
*	\brief 矩形Trigger
*/
class RectTrigger : public Trigger
{
public:
	RectTrigger(const std::string& name, int layer, const Point2& pos, const Rect& rect);

};

/**
*	\brief 圆形Trigger
*/
class CircleTrigger : public Trigger
{
public:
	CircleTrigger(const std::string& name, int layer, const Point2& pos, int radius);
};

/**
*	\brief 多边形Trigger
*/
class PolygonTrigger : public Trigger
{
public:
	PolygonTrigger(const std::string& name, int layer, const Point2& pos, std::vector<Point2> polygons);
};