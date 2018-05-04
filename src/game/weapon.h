#pragma once

#include "game\item.h"
#include "game\weaponControl.h"

class Equipment;
class Savegame;
class Entity;
class Weapon;

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

	virtual bool Equiped(Entity& entity);
	virtual bool UnEquiped();

	virtual void BeforeAttack();
	virtual void Attack();
	virtual void AfterAttack();
	virtual bool IsAttack();

	void SetAttackAnimation();
	void SetNormalAnimation();

	const std::string& GetWeaponName()const;

	AnimationSpritePtr GetWeaponSprite() {
		return mWeaponSprite;
	}

	Entity* GetEntity() {
		return mEntity;
	}

	virtual bool IsEquiped()const {
		return mEntity;
	}

	virtual bool IsWeapon()const {
		return true;
	}

	virtual const string GetLuaObjectName()const;
private:
	bool mIsEquiped;
	Entity* mEntity;
	AnimationSpritePtr mWeaponSprite;
	std::unique_ptr<WeaponControlMode> mControl;
};

using WeaponPtr = std::shared_ptr<Weapon>;