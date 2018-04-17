#pragma once

#include"entity\entity.h"
#include"lua\luaRef.h"

#include<functional>

/**
*	\brief Trigger�������������
*/
class Trigger : public Entity
{
public:
	enum TriggerCollisionMode
	{
		TRIGGER_COLLISION_NONE = 0x00,      // Ĭ��ʹ��RECT�����ײ
		TRIGGER_COLLISION_CIRCLE = 0x01,	// Բ�ε���ײ���
		TRIGGER_COLLISION_POLYGON = 0x02,   // ����ε���ײ���
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
	std::function<void()> mFinishCallBack;		/** �����ص����� */

};

/**
*	\brief ����Trigger
*/
class RectTrigger : public Trigger
{
public:
	RectTrigger(const std::string& name, int layer, const Point2& pos, const Rect& rect);

};

/**
*	\brief Բ��Trigger
*/
class CircleTrigger : public Trigger
{
public:
	CircleTrigger(const std::string& name, int layer, const Point2& pos, int radius);
};

/**
*	\brief �����Trigger
*/
class PolygonTrigger : public Trigger
{
public:
	PolygonTrigger(const std::string& name, int layer, const Point2& pos, std::vector<Point2> polygons);
};