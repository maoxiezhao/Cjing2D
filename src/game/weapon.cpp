#include "weapon.h"
#include "lua\luaContext.h"

/**
*	\TODO 原先的思路是希望对于Gun和Knife分别作为weapon的派生类
*	来设置对应的State状态，但对于这两个派生类来说，仅仅是设置State
*	不同而已，所以可以在不同的Weapon脚本中代替，目前考虑还是先使用
*	不同脚本来实现。
*/
namespace {
	const std::string attackAnimationName = "attack";
	const std::string normalAnimationName = "normal";
}

Weapon::Weapon(const std::string & weaponName, Equipment & equipment) :
	Item(weaponName, equipment),
	mEntity(nullptr),
	mWeaponSprite(nullptr)
{
	mControl = std::make_unique<RotateWeaponControl>(*this);
}

void Weapon::Initialize()
{
	if (IsHasLua())
	{
		const std::string path = std::string("items/weapons/") + GetItemName();
		GetLuaContext().CallFileWithUserdata(path, *this);
	}
}

void Weapon::Update()
{
	Item::Update();
	
	if (mWeaponSprite)
	{
		mWeaponSprite->Update();
		if (mWeaponSprite->IsFrameFinished() && 
			mWeaponSprite->GetCurAnimationName() != normalAnimationName)
			SetNormalAnimation();
	}
	if (mControl)
		mControl->Update();

	//GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUpdate");
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

	// 创建装备Sprite
	const std::string animatePath = "weapons/" + GetItemName();
	mWeaponSprite = entity.CreateAnimationSprite(animatePath, normalAnimationName);
	mWeaponSprite->SetDeferredDraw(true);
	
	auto size = entity.GetSize();
	auto spriteSize = mWeaponSprite->GetSize();
	mWeaponSprite->SetRotateAnchor(0, float(spriteSize.height / 2));
	mWeaponSprite->SetPos({ (int)(size.width * 0.6), size.height /2 });

	SetAttackAnimation();

	mEntity = &entity;
	mIsEquiped = true;

//	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponEquiped");
}

void Weapon::UnEquiped()
{
	if (!IsEquiped())
	{
		Debug::Warning("The Equip:" + GetWeaponName() 
			+ " has already unequiped.");
		return;
	}
	mEntity->RemoveSprite(normalAnimationName);
	mEntity = nullptr;
	mIsEquiped = false;

	//GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
}

void Weapon::BeforeAttack()
{
	//GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
}

void Weapon::Attack()
{
	if (IsEquiped() && !IsAttack())
	{
		SetAttackAnimation();


		//GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
	}
}

void Weapon::AfterAttack()
{
	//GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
}

bool Weapon::IsAttack()
{
	return mWeaponSprite != nullptr ? !mWeaponSprite->IsFrameFinished() : false;
}

void Weapon::SetAttackAnimation()
{
	if (mWeaponSprite) 
	{
		mWeaponSprite->SetCurrAnimation(attackAnimationName);
		mWeaponSprite->StartAnimation();
	}
}

void Weapon::SetNormalAnimation()
{
	if (mWeaponSprite) 
	{
		mWeaponSprite->SetCurrAnimation(normalAnimationName);
		mWeaponSprite->StartAnimation();
	}
}

const std::string & Weapon::GetWeaponName() const
{
	return Item::GetItemName();
}

