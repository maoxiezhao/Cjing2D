#include "trigger.h"
#include "lua\luaContext.h"

Trigger::Trigger(const std::string & name, int layer, const Point2 & pos, TriggerMode triggerMode):
	Entity(name, "Trigger", pos, Size(32, 32), layer),
	mTriggerMode(triggerMode),
	mTriggerTimes(0)
{
}

EntityType Trigger::GetEntityType() const
{
	return EntityType::TRIGGER;
}

const string Trigger::GetLuaObjectName() const
{
	return LuaContext::module_trigger_name;
}

void Trigger::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	if (CanNotifyTrigger(otherEntity))
		NotifyTrigger(otherEntity);
}

Trigger::TriggerMode Trigger::GetTriggerType() const
{
	return mTriggerMode;
}

void Trigger::SetTriggerTimes(int times)
{
	mTriggerTimes = times;
}

bool Trigger::CanNotifyTrigger(Entity & otherEntity)
{
	return (mTriggerTimes > 0 || mTriggerTimes == -1 ) && GetLuaContext()->CallFunctionWithUserdata(*this, "OnCanNotifyTrigger",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, otherEntity);
		return 1;
	});;
}

void Trigger::NotifyTrigger(Entity & otherEntity)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyTrigger",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, otherEntity);
		return 1;
	});

	// ´¥·¢´ÎÊý¼õ1
	if(mTriggerTimes != -1)
		mTriggerTimes--;
}

RectTrigger::RectTrigger(const std::string & name, int layer, 
	const Point2 & pos, const Size & size):
	Trigger(name, layer, pos, TriggerMode::TRIGGER_COLLISION_NONE)
{
	SetSize(size);
	SetDrawOnYOrder(true);
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);
}

