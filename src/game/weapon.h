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
	virtual void Droped();

	virtual bool BeforeAttack();
	virtual void Attack();
	virtual void AfterAttack();
	virtual bool IsAttack()const;
	virtual bool IsCanAttack()const;

	const std::string& GetWeaponName()const;
	virtual bool IsEquiped()const;
	virtual bool IsWeapon()const;

	void SetAnimation(const std::string& name);
	void SetAttackAnimation();
	void SetNormalAnimation();
	AnimationSpritePtr GetWeaponSprite();
	Entity* GetEntity();
	void SetAttackDelta(uint32_t delta);
	uint32_t GetAttackDelta()const;

	virtual const string GetLuaObjectName()const;

private:
	bool mIsEquiped;			/** �Ƿ�װ�� */
	uint32_t mAttackDelta;		/** ������� */
	uint32_t mNextAttackDate;	/** �´ι���ʱ�� */
	bool mCanAttack;			/** �Ƿ���Թ��� */

	Entity* mEntity;
	AnimationSpritePtr mWeaponSprite;
	std::unique_ptr<WeaponControlMode> mControl;

};

using WeaponPtr = std::shared_ptr<Weapon>;