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
	enum TriggerMode
	{
		TRIGGER_COLLISION_NONE = 0x00,      // Ĭ��ʹ��RECT�����ײ
		TRIGGER_COLLISION_CIRCLE = 0x01,	// Բ�ε���ײ���
		TRIGGER_COLLISION_POLYGON = 0x02,   // ����ε���ײ���
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
	int mTriggerTimes;			/** �����������Ĵ�������Ϊ-1ʱ�������޴��� */
};

/**
*	\brief ����Trigger
*/
class RectTrigger : public Trigger
{
public:
	RectTrigger(const std::string& name, int layer, const Point2& pos, const Size& size);
};

