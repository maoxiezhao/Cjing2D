#include "weapon.h"
#include "lua\luaContext.h"
#include "core\system.h"
#include "entity\entity.h"
#include "entity\entities.h"
#include "entity\player.h"
#include "entity\enemy.h"

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
	mCanAttack(true),
	mIsEquiped(false),
	mNotifyCollision(false),
	mStopAttackDelay(200),
	mStopAttackDate(0)
{
	mControl = std::make_unique<RotateWeaponControl>(*this);
	mWeaponInstance = std::make_shared<WeaponInstance>(weaponName + "_instance", 0, *this);
	mWeaponInstance->SetNotifyCollision(mNotifyCollision);
	mWeaponInstance->SetEnable(false);
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

	uint32_t now = System::Now();
	if (mIsAttack)
	{
		if (now >= mStopAttackDate)
		{
			mIsAttack = false;
			if (mWeaponInstance)
				mWeaponInstance->SetEnable(false);
		}
	}

	if ( now >= mNextAttackDate)
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
		Debug::Warning("The Equip:" + GetWeaponName() + " has already equiped.");
		return false;
	}

	// ���ص�װ����entity��
	auto attachPos = entity.GetAttachPos() + mSpritePosOffset;
	entity.AddAttachEntity(mWeaponInstance, attachPos);

	// ����װ��Sprite
	const std::string animatePath = "weapons/" + GetItemName();
	if(mWeaponInstance->GetSprite(animatePath) == nullptr )
		CreateWeaponSprite(entity, animatePath);

	mEntity = &entity;
	mIsEquiped = true;

	SetNormalAnimation();
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

	mEntity->RemoveAttachEntity(mWeaponInstance);
	mIsEquiped = false;

	return true;
}

/**
*	\brief ����������
*/
void Weapon::Droped()
{
	GetLuaContext().CallFunctionWithUserdata(*this, "OnWeaponDroped");
	mEntity = nullptr;
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
			// ���ù�����״̬
			mIsAttack = true;	
			mWeaponInstance->SetEnable(true);
			mStopAttackDate = System::Now() + mStopAttackDelay;

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
	//return  (mWeaponSprite != nullptr ? 
	//	!mWeaponSprite->IsFrameFinished() : false) || mIsAttack;
	return mIsAttack;
}

bool Weapon::IsCanAttack() const
{
	return mCanAttack && !IsAttack() && !mIsAttack;
}

void Weapon::SetVisible(bool visible)
{
	mWeaponInstance->SetVisible(visible);
	mWeaponSprite->SetVisible(visible);
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

std::string Weapon::GetWeaponName() const
{
	return Item::GetItemName();
}

const string Weapon::GetLuaObjectName() const
{
	return LuaContext::module_weapon_name;
}

void Weapon::CreateWeaponSprite(Entity& entity, const std::string& path)
{
	mWeaponSprite = mWeaponInstance->CreateAnimationSprite(path, normalAnimationName);
	mWeaponSprite->SetDeferredDraw(true);

	auto size = entity.GetSize();
	auto spriteSize = mWeaponSprite->GetSize();
	Point2 rotateAnchor = { 0, spriteSize.height / 2 };
	rotateAnchor += mSpriteRotateOffset;
	
	mWeaponSprite->SetRotateAnchor((float)rotateAnchor.x, (float)rotateAnchor.y);
	mWeaponInstance->SetSize(spriteSize);
	mWeaponInstance->SetBoundRotateAnchor((float)rotateAnchor.x, (float)rotateAnchor.y);
}

AnimationSpritePtr Weapon::GetWeaponSprite() 
{
	return mWeaponSprite;
}

Entity* Weapon::GetEntity() 
{
	return mEntity;
}

Entity & Weapon::GetWeaponInstance()
{
	return *mWeaponInstance;
}

void Weapon::SetAttackDelta(uint32_t delta)
{
	mAttackDelta = delta;
}

uint32_t Weapon::GetAttackDelta() const
{
	return uint32_t();
}

/**
*	\brief ������������λ��ƫ�ƣ�Ӧ��Equipǰ����
*/
void Weapon::SetSpritePosOffset(const Point2 & offset)
{
	mSpritePosOffset = offset;
}

void Weapon::SetSpriteRotateOffset(const Point2 & offset)
{
	mSpriteRotateOffset = offset;
}

void Weapon::SetWeaponControlEnable(bool enable)
{
	if (mControl != nullptr)
		mControl->SetEnable(enable);
}

bool Weapon::IsWeaponFliped() const
{
	return mControl != nullptr ? mControl->IsFlip() : false;
}

void Weapon::SetNotifyCollision(bool collisioned)
{
	mNotifyCollision = collisioned;
	mWeaponInstance->SetNotifyCollision(collisioned);
}

bool Weapon::IsNotifyCollision() const
{
	return mNotifyCollision;
}

void Weapon::SetStopAttackDelay(uint32_t delay)
{
	mStopAttackDelay = delay;
}

void Weapon::ComputeDemage(Entity & entity)
{
	if (IsAttack())
	{
		GetLuaContext().CallFunctionWithUserdata(*this, "OnComputeDemage",
			[&](lua_State*l)->int {
			GetLuaContext().PushUserdata(l, entity);
			return 1;
		});
	}
}

void Weapon::NotifyAttackPlayer(Player & enemy)
{
	GetLuaContext().CallFunctionWithUserdata(*this, "OnTryAttackPlayer",
		[&](lua_State*l)->int {
		GetLuaContext().PushUserdata(l, enemy);
		return 1;
	});
}

void Weapon::NotifyAttackEnemy(Enemy & enemy)
{
	GetLuaContext().CallFunctionWithUserdata(*this, "OnTryAttackEnemey",
		[&](lua_State*l)->int {
		GetLuaContext().PushUserdata(l, enemy);
		return 1;
	});
}

bool Weapon::IsEquiped()const 
{
	return mEntity && mIsEquiped;
}

bool Weapon::IsWeapon()const 
{
	return true;
}