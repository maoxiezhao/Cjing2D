#pragma once

#include"entity\entity.h"
#include"entity\entityAttack.h"

class Game;

/**
*	\brief Enemey.
*/
class Enemy : public Entity
{
public:
	Enemy(Game& game, const std::string& name, const std::string& templName, int layer, const Point2& pos);
	static EntityPtr Create(Game& game, const std::string& name, const std::string& templName, int layer, const Point2& pos);

	/** system */
	virtual void Update();
	virtual void Draw();
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

	/** notify */
	virtual void NotifyBeforeCreated();
	virtual void NotifyAfterCreated();
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual void NotifyHurt(Entity& source);
	virtual void NotifyKilled();

	/** status */
	bool IsKilled()const;
	bool IsKilledAnimationFinished()const;
	virtual bool IsObstacle(Entity& entity)const;
	virtual bool IsObstacleEnemy()const;
	virtual bool IsObstaclePlayer()const;
	void SetCurAnimation(const std::string& name);

	/** attack */
	void SetAttackReaction(EntityAttack attack, EntityReactionType type);
	void SetDefaultAttackReactions();

	void Restart();
	void TryHurt(EntityAttack attack, Entity& source);
	void CustomAttack(Entity& source, EntityAttack attack);
	void Hurt(Entity& source);
	void Kill();

private:
	Game& mGame;

	bool mIsHurting;
	bool mIsOnlyUpdateInLua;	/** 仅进行lua更新 */
	bool mIsImmobilized;

	bool mCanAttack;
	uint32_t mHurtProtectedTime;/** 受伤时无敌时间 */
	uint32_t mCanAttackDate;	/** 再次可以攻击的时间 */

	/** Enemy Defualt Property */
	/** 这些属性属于默认设置的，可以在Lua中另外设置并计算 */
	int mLife;
	int mDemage;

	std::map<EntityAttack, EntityReactionType> mAttackReactions;
};