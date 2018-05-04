#include "entity\bullet.h"
#include "entity\player.h"
#include "entity\enemy.h"
#include "lua\luaContext.h"
#include "core\system.h"
#include "movements\movement.h"

/**
*	\brief 子弹的定义为仅有一个方向sprite，因此对于不同的
*	方向只是设置sprite的角度和obb的角度
*	
*	bullet因为同屏数量巨大，所以不使用像素级碰撞检测
*/

Bullet::Bullet(const std::string & templName, int layer):
	Entity("", templName, Point2(), Size(), layer),
	mBulletType(BULLET_TYPE::BULLET_PLAYER),
	mStopNow(false)
{
	// 设置默认属性
	SetDemage(1);
	SetSize({ 16, 16 });
	SetFacingDegree(0.0f);
	SetDrawOnYOrder(true);

	mDisappearDate = System::Now() + 10000;
}

EntityPtr Bullet::Create(const std::string & templName, int layer)
{
	auto bullet = std::make_shared<Bullet>(templName, layer);
	return bullet;
}

void Bullet::Update()
{
	Entity::Update();

	if (IsSuspended())
		return;

	// 判断是否结束
	if (IsStop())
	{
		mDisappearDate = System::Now();
	}

	uint32_t now = System::Now();
	if (now > mDisappearDate)
		RemoveFromMap();
}

EntityType Bullet::GetEntityType() const
{
	return EntityType::BULLET;
}

const string Bullet::GetLuaObjectName() const
{
	return LuaContext::module_entity_bullet_name;
}

void Bullet::NotifyBeforeCreated()
{
	std::string path = "items/weapons/" + GetTemplName() + ".lua";
	GetLuaContext()->CallFileWithUserdata(path, *this);
}

void Bullet::NotifyAfterCreated()
{
}

/**
*	\brief 响应事件当bullet与enemy碰撞，如果子弹类型是Player则造成伤害
*/
void Bullet::NotifyCollisionWithEnemy(Enemy & enemy)
{
	if (mBulletType == BULLET_TYPE::BULLET_PLAYER)
	{
		if (IsFiring())
			enemy.TryHurt(EntityAttack::BULLET, *this);
	}
	else if (mBulletType == BULLET_TYPE::BULLET_CUSTOM) {
		GetLuaContext()->CallFunctionWithUserdata(*this, "OnTryAttackEnemy",
			[&](lua_State*l)->int {
			GetLuaContext()->PushUserdata(l, enemy);
			return 1;
		});
	}

}

/**
*	\brief 响应事件当bullet与enemy碰撞，如果子弹类型是Player则造成伤害
*/
void Bullet::NotifyCollisionWithPlayer(Player & player)
{
	if (mBulletType == BULLET_TYPE::BULLET_ENEMY)
	{
		if (IsFiring());
	}
	else if (mBulletType == BULLET_TYPE::BULLET_CUSTOM) {
		GetLuaContext()->CallFunctionWithUserdata(*this, "OnTryAttackPlayer",
			[&](lua_State*l)->int {
			GetLuaContext()->PushUserdata(l, player);
			return 1;
		});
	}

}

bool Bullet::IsObstacle(Entity & entity) const
{
	return false;
}

bool Bullet::IsObstacleEnemy() const
{
	return false;
}

bool Bullet::IsObstaclePlayer() const
{
	return false;
}

void Bullet::SetBulletType(BULLET_TYPE type)
{
	mBulletType = type;
}

int Bullet::GetBulletType() const
{
	return static_cast<int>(mBulletType);
}

/**
*	\brief 判断子弹行为是否结束
*/
bool Bullet::IsStop()
{
	if (mStopNow)
		return true;

	// 对于自定义bullet而言，始终返回false，需要在脚本中显式调用Stop
	if (mBulletType == BULLET_TYPE::BULLET_CUSTOM)
	{
		GetLuaContext()->CallFunctionWithUserdata(*this, "OnIsStop()");
		return false;
	}
	return (GetMovement() == nullptr || GetMovement()->IsFinished());
}

/**
*	\brief 是否是开火有效的
*/
bool Bullet::IsFiring() const
{
	return !mStopNow && (GetMovement() != nullptr && !GetMovement()->IsFinished());
}

void Bullet::SetAliveTime(uint32_t time)
{
	mDisappearDate = System::Now() + time;
}

/**
*	\brief 获取当前剩余生命时间
*/
uint32_t Bullet::GetAliveTime() const
{
	return (mDisappearDate - System::Now());
}

/**
*	\brief 设置当前伤害
*/
void Bullet::SetDemage(int demage)
{
	mDemage = demage;
}

int Bullet::GetDemage() const
{
	return mDemage;
}
