#pragma once

#include"entity\entity.h"

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
	virtual void NotifyHurt();
	virtual void NotifyKilled();

	/** status */
	bool IsKilled()const;
	bool IsKilledAnimationFinished()const;
	virtual bool IsObstacle(Entity& entity)const;
	virtual bool IsObstacleEnemy()const;
	virtual bool IsObstaclePlayer()const;

	/** attack */
	void Restart();
	void TryHurt();
	void Hurt();
	void Kill();

private:
	Game& mGame;

	bool mIsHurting;
	bool mIsOnlyUpdateInLua;	/** 仅进行lua更新 */
	bool mIsImmobilized;
	bool mIsPushed;

	/** Enemy Defualt Property */
	/** 这些属性属于默认设置的，可以在Lua中另外设置并计算 */
	int mLife;
	int mDemage;
};