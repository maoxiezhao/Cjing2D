#pragma once

#include "game\item.h"
#include "game\weaponControl.h"
#include "entity\weaponInstance.h"

class Equipment;
class Savegame;
class Entity;
class Weapon;
class Enemey;
class Player;

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

	std::string GetWeaponName()const;
	virtual bool IsEquiped()const;
	virtual bool IsWeapon()const;

	void SetAnimation(const std::string& name);
	void SetAttackAnimation();
	void SetNormalAnimation();
	AnimationSpritePtr GetWeaponSprite();
	Entity* GetEntity();
	Entity& GetWeaponInstance();
	void SetAttackDelta(uint32_t delta);
	uint32_t GetAttackDelta()const;
	void SetSpritePosOffset(const Point2& offset);
	void SetSpriteRotateOffset(const Point2& offset);
	void SetWeaponControlEnable(bool enable);
	bool IsWeaponFliped()const;
	void SetNotifyCollision(bool collisioned);
	bool IsNotifyCollision()const;
	void SetStopAttackDelay(uint32_t delay);

	void NotifyAttackPlayer(Player & enemy);
	void NotifyAttackEnemy(Enemy & enemy);
	virtual const string GetLuaObjectName()const;

private:
	void CreateWeaponSprite(Entity& entity, const std::string& path);

private:
	bool mIsEquiped;			/** 是否装备 */
	uint32_t mAttackDelta;		/** 攻击间隔 */
	uint32_t mNextAttackDate;	/** 下次攻击时间 */
	bool mCanAttack;			/** 是否可以攻击 */
	Point2 mSpritePosOffset;	/** 精灵位置偏移 */
	Point2 mSpriteRotateOffset; /** 精灵选择偏移 */
	bool mNotifyCollision;		/** 是否响应碰撞 */

	bool mIsAttack;				/** 是否在攻击,一般用是否播放完帧动画做攻击结束
								判定，此参数用于额外的控制的有效攻击时间 */
	uint32_t mStopAttackDelay;
	uint32_t mStopAttackDate;

	Entity* mEntity;
	AnimationSpritePtr mWeaponSprite;
	std::unique_ptr<WeaponControlMode> mControl;
	std::shared_ptr<WeaponInstance> mWeaponInstance;
};

using WeaponPtr = std::shared_ptr<Weapon>;