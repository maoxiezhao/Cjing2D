#include "weapon.h"
#include "lua\luaContext.h"
#include "core\system.h"

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
	mWeaponSprite(nullptr),
	mAttackDelta(500),
	mNextAttackDate(0),
	mCanAttack(true)
{
	mControl = std::make_unique<RotateWeaponControl>(*this);
}

void Weapon::Initialize()
{
	if (IsHasLua())
	{
		const std::string path = std::string("items/weapons/") + GetItemName();
		GetLuaContext().CallFileWithUserdata(path, *this);
		GetLuaContext().CallFunctionWithUserdata(*this, "OnCreated");
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

	if (System::Now() >= mNextAttackDate)
		mCanAttack = true;

	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUpdate");
}

void Weapon::Uninitialize()
{
	Item::Uninitialize();
}

bool Weapon::Equiped(Entity & entity)
{
	if (IsEquiped())
	{
		Debug::Warning("The Equip:" + GetWeaponName()
			+ " has already equiped.");
		return false;
	}

	// 创建装备Sprite
	const std::string animatePath = "weapons/" + GetItemName();
	mWeaponSprite = entity.CreateAnimationSprite(animatePath, normalAnimationName);
	mWeaponSprite->SetDeferredDraw(true);
	
	auto size = entity.GetSize();
	auto spriteSize = mWeaponSprite->GetSize();
	auto attachPos = entity.GetAttachPos();
	mWeaponSprite->SetRotateAnchor(0, float(spriteSize.height / 2));
	mWeaponSprite->SetPos(attachPos);

	SetAttackAnimation();

	mEntity = &entity;
	mIsEquiped = true;

	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponEquiped");
	return true;
}

bool Weapon::UnEquiped()
{
	if (!IsEquiped())
	{
		Debug::Warning("The Equip:" + GetWeaponName() 
			+ " has already unequiped.");
		return false;
	}

	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");

	mEntity->RemoveSprite(normalAnimationName);
	mEntity = nullptr;
	mIsEquiped = false;
	return true;
}

bool Weapon::BeforeAttack()
{
	return GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponBeforeAttack");
}

/**
*	\brief weapon攻击动作，在Player::Attack调用
*/
void Weapon::Attack()
{
	if (IsEquiped() && IsCanAttack())
	{
		// 在lua中再判断依次
		bool canAttack = BeforeAttack();
		if (canAttack)
		{
			GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponAttack");
			AfterAttack();
		}
	}
}

void Weapon::AfterAttack()
{
	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponAfterAttack");

	mCanAttack = false;
	mNextAttackDate = System::Now() + mAttackDelta;
}

bool Weapon::IsAttack()const
{
	return  (mWeaponSprite != nullptr ? 
		!mWeaponSprite->IsFrameFinished() : false);
}

bool Weapon::IsCanAttack() const
{
	return mCanAttack && !IsAttack();
}

/**
*	\brief 设置weapon动画
*/
void Weapon::SetAnimation(const std::string & name)
{
	if (mWeaponSprite)
	{
		mWeaponSprite->SetCurrAnimation(name);
		mWeaponSprite->StartAnimation();
	}
}

void Weapon::SetAttackAnimation()
{
	SetAnimation(attackAnimationName);
}

void Weapon::SetNormalAnimation()
{
	SetAnimation(normalAnimationName);
}

const std::string & Weapon::GetWeaponName() const
{
	return Item::GetItemName();
}

const string Weapon::GetLuaObjectName() const
{
	return LuaContext::module_weapon_name;
}

AnimationSpritePtr Weapon::GetWeaponSprite() 
{
	return mWeaponSprite;
}

Entity* Weapon::GetEntity() 
{
	return mEntity;
}

void Weapon::SetAttackDelta(uint32_t delta)
{
	mAttackDelta = delta;
}

uint32_t Weapon::GetAttackDelta() const
{
	return uint32_t();
}

bool Weapon::IsEquiped()const 
{
	return mEntity;
}

bool Weapon::IsWeapon()const 
{
	return true;
}