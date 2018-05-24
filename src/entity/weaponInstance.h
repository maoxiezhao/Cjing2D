#pragma once

#include "entity\entity.h"

class Weapon;
/**
*	\brief 子弹实体类
*/
class WeaponInstance : public Entity
{
public:
	enum class WEAPON_TYPE
	{
		BULLET_PLAYER = 0,
		BULLET_ENEMY,
		BULLET_CUSTOM,
	};

	WeaponInstance(const std::string& name, int layer, Weapon& weapon);

	virtual void Update();
	virtual void Draw();
	virtual void SetFacingDegree(float degree);

	/** lua */
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

	/** notify */
	virtual void NotifyCollisionWithEnemy(Enemy& enemy);
	virtual void NotifyCollisionWithPlayer(Player& player);

	/** status */
	Weapon& GetWeapon();
	void SetNotifyCollision(bool notified);

private:
	Weapon& mWeapon;

};