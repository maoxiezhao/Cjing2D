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

	/** ��С�Ȼ���������lua������ */
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

	/** ��������нű����£���ֱ����ֹ */
	if (!mIsOnlyUpdateInLua)
	{
		/** �����ж��Ƿ�����������������Զ��Ƴ�  */
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
*	\brief ����ǰִ�У���ʱȥִ�ж�Ӧ��Lua�ű�
*/
void Enemy::NotifyBeforeCreated()
{
	std::string path = "enemies/" + GetTemplName() + ".lua";
	GetLuaContext()->CallFileWithUserdata(path, *this);
}

/**
*	\brief ������ִ�У�����������һЩ���������
*/
void Enemy::NotifyAfterCreated()
{
	// ���sprite��OnCreate�д�����Լ����
	// ��ʱ��Ӧ������ͳһ�ķ���
	Direction4 dir = GetDirection();
	for (const auto& nameSprite : GetSprites())
	{
		auto& sprite = nameSprite.sprite;
		if (sprite && sprite->IsAnimationed()) {
			auto& animateSprite = dynamic_cast<AnimationSprite&>(*sprite);
			animateSprite.SetCurrDirection(dir);
		}
	}
	// ���õ�ǰ״̬
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
*	\brief ���õ�ǰEnemy״̬,��enemy���������enable״̬�ı�ʱ����
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
