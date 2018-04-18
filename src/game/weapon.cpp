#include "weapon.h"

namespace {
	const std::string attackAnimationName = "attack";
	const std::string normalAnimationName = "normal";
}

Weapon::Weapon(const std::string & weaponName, Equipment & equipment) :
	Item(weaponName, equipment),
	mEntity(nullptr),
	mWeaponSprite(nullptr)
{
}

void Weapon::Initialize()
{
	//Item::Initialize();
}

void Weapon::Update()
{
	Item::Update();
}

void Weapon::Uninitialize()
{
	Item::Uninitialize();
}

void Weapon::Equiped(Entity & entity)
{
	if (IsEquiped())
	{
		Debug::Warning("The Equip:" + GetWeaponName()
			+ " has already equiped.");
		return;
	}

	mEntity = &entity;
	mIsEquiped = true;
}

void Weapon::UnEquiped()
{
	if (!IsEquiped())
	{
		Debug::Warning("The Equip:" + GetWeaponName() 
			+ " has already unequiped.");
		return;
	}
	mEntity = nullptr;
	mIsEquiped = false;
}

void Weapon::Attack()
{
	if (IsEquiped())
	{
	}
}

void Weapon::SetAttackAnimation()
{
}

void Weapon::SetNormalAnimation()
{
}

const std::string & Weapon::GetWeaponName() const
{
	return Item::GetItemName();
}
