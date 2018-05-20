#include "weapon.h"
#include "lua\luaContext.h"
#include "core\system.h"

/**
*	\TODO ԭ�ȵ�˼·��ϣ������Gun��Knife�ֱ���Ϊweapon��������
*	�����ö�Ӧ��State״̬����������������������˵������������State
*	��ͬ���ѣ����Կ����ڲ�ͬ��Weapon�ű��д��棬Ŀǰ���ǻ�����ʹ��
*	��ͬ�ű���ʵ�֡�
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

	// ����װ��Sprite
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
*	\brief weapon������������Player::Attack����
*/
void Weapon::Attack()
{
	if (IsEquiped() && IsCanAttack())
	{
		// ��lua�����ж�����
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
*	\brief ����weapon����
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