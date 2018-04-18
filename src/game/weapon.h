#pragma once

#include "game\item.h"

class Equipment;
class Savegame;
class Entity;

class WeaponData
{
public:
};

class Weapon : public Item
{
public:
	Weapon(const std::string& weaponName, Equipment& equipment);

	virtual void Initialize();
	virtual void Update();
	virtual void Uninitialize();

	virtual void Equiped(Entity& entity);
	virtual void UnEquiped();
	virtual void Attack();

	void SetAttackAnimation();
	void SetNormalAnimation();

	const std::string& GetWeaponName()const;
	Entity* GetEntity() {
		return mEntity;
	}

	virtual bool IsEquiped()const {
		return mEntity;
	}

	virtual bool IsWeapon()const {
		return true;
	}
private:
	bool mIsEquiped;
	Entity* mEntity;
	AnimationSpritePtr mWeaponSprite;
};

using WeaponPtr = std::shared_ptr<Weapon>;