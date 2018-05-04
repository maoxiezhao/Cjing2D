#pragma once

#include "entity\entity.h"

/**
*	\brief 子弹实体类
*/
class Bullet : public Entity
{
public:
	enum class BULLET_TYPE
	{
		BULLET_PLAYER = 0,
		BULLET_ENEMY,
		BULLET_CUSTOM,
	};

	Bullet(const std::string& templName, int layer);
	static EntityPtr Create(const std::string& templName, int layer);

	/** system */
	virtual void Update();
	virtual EntityType GetEntityType()const;
	virtual const string GetLuaObjectName()const;

	/** notify */
	virtual void NotifyBeforeCreated();
	virtual void NotifyAfterCreated();
	virtual void NotifyCollisionWithEnemy(Enemy& enemy);
	virtual void NotifyCollisionWithPlayer(Player& player);

	/** status */
	virtual bool IsObstacle(Entity& entity)const;
	virtual bool IsObstacleEnemy()const;
	virtual bool IsObstaclePlayer()const;

	void SetBulletType(BULLET_TYPE type);
	int GetBulletType()const;
	bool IsStop();
	bool IsFiring()const;
	void SetAliveTime(uint32_t time);
	uint32_t GetAliveTime()const;
	
	/** extra status */
	void SetDemage(int demage);
	int GetDemage()const;

private:
	BULLET_TYPE mBulletType;
	uint32_t mDisappearDate;
	uint32_t mDisappearAnimTime;

	bool mStopNow;
	int mDemage;
};