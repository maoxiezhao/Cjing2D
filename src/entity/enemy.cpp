#include"entity\enemy.h"
#include"game\combat.h"
#include"game\animationSprite.h"
#include"lua\luaContext.h"

Enemy::Enemy(Game & game, const std::string & name, const std::string& templName, int layer, const Point2 & pos):
	Entity(name, templName, pos, Size(0,0), layer),
	mGame(game),
	mLife(1),
	mDemage(1),
	mIsHurting(false),
	mIsImmobilized(false),
	mIsPushed(false),
	mIsOnlyUpdateInLua(false)
{
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);
	SetDrawOnYOrder(true);

	/** 大小等基本属性在lua中设置 */
	SetSize({ 32, 32 });
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

	/** 如果仅进行脚本更新，则直接终止 */
	if (!mIsOnlyUpdateInLua)
	{
		/** 程序判断是否死亡，如果死亡则自动移除  */
		if (IsKilled() && IsKilledAnimationFinished())
		{
			RemoveFromMap();
			NotifyKilled();
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
	std::string path = "enemies/" + GetTemplName() + ".lua";
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
	return false;
}

bool Enemy::IsKilledAnimationFinished() const
{
	return false;
}

bool Enemy::IsObstacle(Entity & entity) const
{
	return entity.IsObstacleEnemy();
}

bool Enemy::IsObstacleEnemy() const
{
	return false;
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

void Enemy::TryHurt()
{
}

void Enemy::Hurt()
{
}

void Enemy::Kill()
{
}

void Enemy::NotifyHurt()
{
}
