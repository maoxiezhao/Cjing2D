#include "weaponInstance.h"
#include "game\weapon.h"
#include "lua\luaContext.h"

WeaponInstance::WeaponInstance(const std::string & name, int layer, Weapon& weapon):
	Entity(name, "", Point2(), Size(), layer),
	mWeapon(weapon)
{
	SetSize({ 16, 16 });
	SetFacingDegree(0.0f);
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);
}

void WeaponInstance::Update()
{
	Entity::Update();

	auto& weapon = GetWeapon();
	if (weapon.IsEquiped() && weapon.IsAttack() && weapon.IsNotifyCollision())
	{
		CheckCollisionWithEntities();
	}
}

void WeaponInstance::Draw()
{
	Entity::Draw();
}

void WeaponInstance::SetFacingDegree(float degree)
{
	Entity::SetFacingDegree(degree);
	SetBoundingAngle(degree);
}

EntityType WeaponInstance::GetEntityType() const
{
	return EntityType::WEAPON;
}

const string WeaponInstance::GetLuaObjectName() const
{
	return LuaContext::module_weapon_instance;
}

void WeaponInstance::NotifyCollisionWithEnemy(Enemy & enemy)
{
	Weapon& weapon = GetWeapon();
	weapon.NotifyAttackEnemy(enemy);
}

void WeaponInstance::NotifyCollisionWithPlayer(Player & player)
{
	Weapon& weapon = GetWeapon();
	weapon.NotifyAttackPlayer(player);
}

Weapon & WeaponInstance::GetWeapon()
{
	return mWeapon;
}

/**
*	\brief 设置是否响应Collision事件
*/
void WeaponInstance::SetNotifyCollision(bool notified)
{
	CollisionMode mode = notified ? CollisionMode::COLLISION_OVERLAPING :
		CollisionMode::COLLISION_NONE;
	SetCollisionMode(mode);
}
