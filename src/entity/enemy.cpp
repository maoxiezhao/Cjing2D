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

	/** ��С�Ȼ���������lua������ */
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

	/** ����������������ж� */
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

	/** �������� */
	if (mLife > 0 && !mCanAttack && now >= mCanAttackDate)
		mCanAttack = true;

	/** ��������нű����£���ֱ����ֹ */
	if (!mIsOnlyUpdateInLua)
	{
		/** �����ж��Ƿ�����������������Զ��Ƴ�  */
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
*	\brief ����ǰִ�У���ʱȥִ�ж�Ӧ��Lua�ű�
*/
void Enemy::NotifyBeforeCreated()
{
	std::string path = "entities/enemies/" + GetTemplName() + ".lua";
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
*	\brief ��������ʱ���Ŷ�����ʱ�䣬��������ǰ����
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
*	\brief ���õ�ǰEnemy״̬,��enemy���������enable״̬�ı�ʱ����
*/
void Enemy::Restart()
{
	if (IsKilled())	
		return;
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnRestart");
}

/**
*	\brief ���öԹ�������Ӧ��Ϊ
*/
void Enemy::SetAttackReaction(EntityAttack attack, EntityReactionType type)
{
	mAttackReactions[attack] = type;
}

/**
*	\brief ����Ĭ�ϵĹ�����Ӧ��Ϊ
*/
void Enemy::SetDefaultAttackReactions()
{
	SetAttackReaction(EntityAttack::COMBAT, EntityReactionType::REACTION_HURT);
	SetAttackReaction(EntityAttack::BULLET, EntityReactionType::REACTION_HURT);
	SetAttackReaction(EntityAttack::BOOMERANG, EntityReactionType::REACTION_HURT);
}

/**
*	\brief �Ե��˽����˺��ж�
*/
void Enemy::TryHurt(EntityAttack attack, Entity & source)
{
	auto reaction = mAttackReactions[attack];
	if (reaction == EntityReactionType::REACTION_IGNORE)
		return;

	// ��Ӧ��Ӧ�Ĺ�����Ӧ��Ϊ
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
*	\brief �ڽű��м���Enemy���ܵ�����ʱ����Ϊ
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
*	\brief ����ʱ��ϵͳ�������Ϊ
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
*	\brief Enemey��ɱ��
*/
void Enemy::Killed()
{
	mLife = 0;
	mIsKilled = true;
	mRemoveDate = System::Now() + mDeadAnimTime;
}

/**
*	\brief ����ʱ�ű��������Ϊ
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
