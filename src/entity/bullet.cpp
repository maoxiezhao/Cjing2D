#include "entity\bullet.h"
#include "entity\player.h"
#include "entity\enemy.h"
#include "lua\luaContext.h"
#include "core\system.h"
#include "movements\movement.h"

/**
*	\brief �ӵ��Ķ���Ϊ����һ������sprite����˶��ڲ�ͬ��
*	����ֻ������sprite�ĽǶȺ�obb�ĽǶ�
*	
*	bullet��Ϊͬ�������޴����Բ�ʹ�����ؼ���ײ���
*/

Bullet::Bullet(const std::string & templName, int layer):
	Entity("", templName, Point2(), Size(), layer),
	mBulletType(BULLET_TYPE::BULLET_PLAYER),
	mStopNow(false)
{
	// ����Ĭ������
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

	// �ж��Ƿ����
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
*	\brief ��Ӧ�¼���bullet��enemy��ײ������ӵ�������Player������˺�
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
*	\brief ��Ӧ�¼���bullet��enemy��ײ������ӵ�������Player������˺�
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
*	\brief �ж��ӵ���Ϊ�Ƿ����
*/
bool Bullet::IsStop()
{
	if (mStopNow)
		return true;

	// �����Զ���bullet���ԣ�ʼ�շ���false����Ҫ�ڽű�����ʽ����Stop
	if (mBulletType == BULLET_TYPE::BULLET_CUSTOM)
	{
		GetLuaContext()->CallFunctionWithUserdata(*this, "OnIsStop()");
		return false;
	}
	return (GetMovement() == nullptr || GetMovement()->IsFinished());
}

/**
*	\brief �Ƿ��ǿ�����Ч��
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
*	\brief ��ȡ��ǰʣ������ʱ��
*/
uint32_t Bullet::GetAliveTime() const
{
	return (mDisappearDate - System::Now());
}

/**
*	\brief ���õ�ǰ�˺�
*/
void Bullet::SetDemage(int demage)
{
	mDemage = demage;
}

int Bullet::GetDemage() const
{
	return mDemage;
}
