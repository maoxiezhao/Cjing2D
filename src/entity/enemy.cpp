#include"entity\enemy.h"
#include"game\combat.h"
#include"lua\luaContext.h"

Enemy::Enemy(Game & game, const std::string & name, int layer, const Point2 & pos):
	Entity(name, pos, Size(0,0), layer),
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

EntityPtr Enemy::Create(Game & game, const std::string & name, int layer, const Point2 & pos)
{
	auto enemyPtr = std::make_shared<Enemy>(game, name, layer, pos);
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
	std::string path = "enemies/" + GetName() + ".lua";
	GetLuaContext()->CallFileWithUserdata(path, *this);
}

/**
*	\brief ������ִ�У�����������һЩ���������
*/
void Enemy::NotifyAfterCreated()
{
}

void Enemy::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
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
