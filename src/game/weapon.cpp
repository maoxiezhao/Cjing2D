#include "weapon.h"
#include "lua\luaContext.h"

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
	mWeaponSprite->SetRotateAnchor(0, float(spriteSize.height / 2));
	mWeaponSprite->SetPos({ (int)(size.width * 0.6), size.height /2 });

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
	mEntity->RemoveSprite(normalAnimationName);
	mEntity = nullptr;
	mIsEquiped = false;

	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
	return true;
}

void Weapon::BeforeAttack()
{
	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
}

void Weapon::Attack()
{
	if (IsEquiped() && !IsAttack())
	{
		SetAttackAnimation();
		GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponAttack");
	}
}

void Weapon::AfterAttack()
{
	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponUnequiped");
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

const string Weapon::GetLuaObjectName() const
{
	return LuaContext::module_weapon_name;
}

