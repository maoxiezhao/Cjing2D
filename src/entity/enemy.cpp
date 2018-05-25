#include"entity\enemy.h"
#include"entity\bullet.h"
#include"entity\weaponInstance.h"
#include"game\combat.h"
#include"game\animationSprite.h"
#include"lua\luaContext.h"
#include"core\system.h"

Enemy::Enemy(Game & game, const std::string & name, const std::string& templName, int layer, const Point2 & pos):
	Entity(name, templName, pos, Size(0,0), layer),
	mGame(game),
	mLife(1),
	mDemage(1),
	mIsHurting(false),
	mIsImmobilized(false),
	mIsOnlyUpdateInLua(false),
	mCanAttack(true),
	mIsKilled(false),
	mHurtProtectedTime(0),
	mDeadAnimTime(2000),
	mRemoveDate(0),
	mHurtTiem(200)
{
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);
	SetDrawOnYOrder(true);

	/** 大小等基本属性在lua中设置 */
	SetSize({ 32, 32 });

	SetDefaultAttackReactions();
}

EntityPtr Enemy::Create(Game & game, const std::string & name, const std::string& templName, int layer, const Point2 & pos)
{
	auto enemyPtr = std::make_shared<Enemy>(game, name, templName, layer, pos);
	return enemyPtr;
}

void Enemy::Update()
{
	Entity::Update();

	if (IsSuspended())
		return;

	uint32_t now = System::Now();

	/** 如果受伤则进行相关判断 */
	if (mIsHurting)
	{
		if (now >= mStopHurtDate)
		{
			mIsHurting = false;
			if (mLife > 0)
			{
				ClearMovements();
				Restart();
			}
			else
			{
				NotifyKilled();
			}
		}
	}

	/** 攻击保护 */
	if (mLife > 0 && !mCanAttack && now >= mCanAttackDate)
		mCanAttack = true;

	/** 如果仅进行脚本更新，则直接终止 */
	if (!mIsOnlyUpdateInLua)
	{
		/** 程序判断是否死亡，如果死亡则自动移除  */
		if (IsKilled() && IsKilledAnimationFinished())
		{
			if (System::Now() >= mRemoveDate) {
				RemoveFromMap();
				//NotifyKilled();
			}
		}
	}
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnUpdate");
}

void Enemy::Draw()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnBeforeDraw");
	Entity::Draw();
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnAfterDraw");
}

EntityType Enemy::GetEntityType() const
{
	return EntityType::ENEMEY;
}

const string Enemy::GetLuaObjectName() const
{
	return LuaContext::module_enemy_name;
}

/**
*	\brief 创建前执行，此时去执行对应的Lua脚本
*/
void Enemy::NotifyBeforeCreated()
{
	std::string path = "entities/enemies/" + GetTemplName() + ".lua";
	GetLuaContext()->CallFileWithUserdata(path, *this);
}

/**
*	\brief 创建后执行，在这里设置一些额外的设置
*/
void Enemy::NotifyAfterCreated()
{
	// 如果sprite在OnCreate中创建（约定）
	// 这时候应该设置统一的方向
	Direction4 dir = GetDirection();
	for (const auto& nameSprite : GetSprites())
	{
		auto& sprite = nameSprite.sprite;
		if (sprite && sprite->IsAnimationed()) {
			auto& animateSprite = dynamic_cast<AnimationSprite&>(*sprite);
			animateSprite.SetCurrDirection(dir);
		}
	}
	// 重置当前状态
	Restart();
}

void Enemy::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	otherEntity.NotifyCollisionWithEnemy(*this);
}

void Enemy::NotifyKilled()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnKilled");
}

bool Enemy::IsKilled() const
{
	return mIsKilled;
}

bool Enemy::IsKilledAnimationFinished() const
{
	return true;
}

bool Enemy::IsObstacle(Entity & entity) const
{
	return entity.IsObstacleEnemy();
}

bool Enemy::IsObstacleEnemy() const
{
	return false;
}

bool Enemy::IsObstaclePlayer() const
{
	return true;
}

void Enemy::SetCurAnimation(const std::string & name)
{
}

void Enemy::SetLife(int life)
{
	if (mLife != life)
	{
		int oldLife = mLife;
		mLife = life;	
		GetLuaContext()->CallFunctionWithUserdata(*this, "OnLifeChange", 
			[&](lua_State*l)->int {
			lua_pushinteger(l, oldLife);
			lua_pushinteger(l, life);
			return 2;
		});
	}
}

int Enemy::GetLife() const
{
	return mLife;
}

/**
*	\brief 设置死亡时播放动画的时间，需在死亡前设置
*/
void Enemy::SetDeadAnimTime(uint32_t time)
{
	mDeadAnimTime = time;
}

bool Enemy::IsHurting() const
{
	return mIsHurting;
}

/**
*	\brief 重置当前Enemy状态,当enemy创建后或者enable状态改变时调用
*/
void Enemy::Restart()
{
	if (IsKilled())	
		return;
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnRestart");
}

/**
*	\brief 设置对攻击的响应行为
*/
void Enemy::SetAttackReaction(EntityAttack attack, EntityReactionType type)
{
	mAttackReactions[attack] = type;
}

/**
*	\brief 设置默认的攻击响应行为
*/
void Enemy::SetDefaultAttackReactions()
{
	SetAttackReaction(EntityAttack::COMBAT, EntityReactionType::REACTION_HURT);
	SetAttackReaction(EntityAttack::BULLET, EntityReactionType::REACTION_HURT);
	SetAttackReaction(EntityAttack::BOOMERANG, EntityReactionType::REACTION_HURT);
}

/**
*	\brief 对敌人进行伤害判定
*/
void Enemy::TryHurt(EntityAttack attack, Entity & source)
{
	auto reaction = mAttackReactions[attack];
	if (reaction == EntityReactionType::REACTION_IGNORE)
		return;

	// 响应对应的攻击反应行为
	switch (reaction)
	{
	case EntityReactionType::REACTION_HURT:
		if (attack == EntityAttack::BULLET)
		{
			auto& bullet = dynamic_cast<Bullet&>(source);
			bullet.ComputeDemage(*this);
		}
		else if (attack == EntityAttack::COMBAT)
		{
			auto& weapon = dynamic_cast<WeaponInstance&>(source);
			weapon.ComputeDemage(*this);
		}

		Hurt(source);
		NotifyHurt(source, attack);
		break;
	case EntityReactionType::REACTION_CUSTOM:
		CustomAttack(source, attack);
		break;
	}

	source.NotifyAttackEnemy(*this, attack, reaction);
}

/**
*	\brief 在脚本中计算Enemy的受到攻击时的行为
*/
void Enemy::CustomAttack(Entity & source, EntityAttack attack)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnBeAttacked",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, source);
		return 1;
	});
}

/**
*	\brief 受伤时的系统处理的行为
*/
void Enemy::Hurt(Entity & source)
{
	auto now = System::Now();
	mCanAttack = false;
	mCanAttackDate = now + mHurtProtectedTime;
	mIsHurting = true;

	ClearMovements();
	SetCurAnimation("Hurt");

	mStopHurtDate = now + mHurtTiem;
}

/**
*	\brief Enemey被杀死
*/
void Enemy::Killed()
{
	mLife = 0;
	mIsKilled = true;
	mRemoveDate = System::Now() + mDeadAnimTime;
}

/**
*	\brief 受伤时脚本处理的行为
*/
void Enemy::NotifyHurt(Entity& source, EntityAttack attack)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnHurt",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, source);
		lua_pushinteger(l, static_cast<int>(attack));
		return 2;
	});
}
