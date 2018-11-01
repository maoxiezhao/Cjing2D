#ifndef _HERO_H_
#define _HERO_H_

#include"common\common.h"
#include"entity\entity.h"
#include"entity\entityAttack.h"
#include"game\animationSprite.h"
#include"game\gameCommands.h"

class PlayerSprite;
class Equipment;

// test
class Weapon;

/**
*	\brief 可操纵的玩家类基类
*/
class Player : public Entity
{
public:
	Player(Equipment& equipment);

	/** system operator */
	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

	/** Notify event */
	virtual void NotifyMovementChanged();
	virtual void NotifyMovementFinished();
	virtual void NotifyPositonChanged();
	virtual void NotifyFacingEntityChanged(Entity* entity);
	virtual void NotifyOverlapEntityChanged(Entity* entity);
	virtual void NotifyAfterCreated();
	virtual void NotifyBeRemoved();

	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual void NotifyCollisionWithEnemy(Enemy& enemy);
	virtual void NotifyCollisionWithBlock(Block& block);
	virtual void NotifyCollisionWithChest(Chest& chest);

	/** status manager */
	void PlaceOnMap(Map& map);
	void CheckPosition();
	void Attack();
	void SetNormalState();
	void SetStopState();
	void NotifyAttack();
	void Grabbing();
	void StopGrabbing();

	/** getter/setter */
	int GetWalkingSpeed()const;
	int GetShiftSpeed()const;
	PlayerSprite& GetPlayerSprites();
	virtual EntityType GetEntityType()const;
	void SetBindDirectoinByGameCommand(bool binded);
	Direction4 GetDirection4()const;
	Direction8 GetDirection8()const;
	bool CanAttack()const;
	bool IsWeaponAttack()const;
	Equipment& GetEquipment();
	const Equipment& GetEquipment()const;
	virtual float GetFacingDegree()const;
	virtual Point2 GetFacingPoint()const;

	virtual bool IsObstacle(Entity& entity)const;
	virtual bool IsObstacleEnemy()const;
	virtual bool IsObstacleBlock()const;
	virtual bool isObstacleChest()const;

	/** Lua Context */
	virtual const string GetLuaObjectName()const;

private:
	std::unique_ptr<PlayerSprite> mPlayerSprites;	// 当前的sprite合集
	Equipment& mEquipment;		// 当前player数值管理者，包括装备管理

	Point2 mDisplayedPos;		// sprite 显示的位置
	bool mIsBindDirectionByGameCommand;

	int mNormalWalkingSpeed;
	int mCurWalkingSpeed;
	int mShiftSpeed;
	bool mCanUseSpActive;

	// test
	std::shared_ptr<Weapon> mWeapon;
};

#endif