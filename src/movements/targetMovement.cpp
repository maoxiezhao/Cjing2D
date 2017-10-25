#include "targetMovement.h"
#include "core\system.h"
#include "utils\geometry.h"
#include "lua\luaContext.h"

namespace {
	const uint32_t refreshMovementDelay = 200;
}

TargetMovement::TargetMovement(const EntityPtr & entity, const Point2 & target, int speed, bool ingoreObstacles):
	StraightMovement(ingoreObstacles),
	mTargetPos(target),
	mMovingSpeed(speed),
	mSignX(0),
	mSignY(0),
	mNextRefreshMovmentDate(System::Now()),
	mTargetEntity(entity),
	mFinished(false)
{
}

void TargetMovement::SetDrawable(Drawable * drawable)
{
	Movement::SetDrawable(drawable);
	RefreshMovment();
	mNextRefreshMovmentDate = System::Now() + refreshMovementDelay;
}

/**
*	\brief �����ƶ������Ŀ��
*	\param entity �����Ŀ���λ��Ϊʵ��λ�ã���ʱposΪ���Ŀ���ƫ�ƣ�
				  ���entityΪ�գ�posΪ�����λ��
*	\param pos  λ�û�ƫ����
*/
void TargetMovement::SetTarget(const EntityPtr& entity, const Point2& pos)
{
	mTargetEntity = entity;
	if (mTargetEntity != nullptr)
	{
		mTargetOffset = pos;
	}
	else
	{
		mTargetPos = pos;
	}
	RefreshMovment();
	mNextRefreshMovmentDate = System::Now() + refreshMovementDelay;
}

/**
*	\brief ����Ŀ��λ��,Ŀ��λ�������ʵ������ܻ�䶯
*/
Point2 TargetMovement::GetTargetPos()const
{
	if (mTargetEntity != nullptr)
	{
		return mTargetEntity->GetPos() + mTargetOffset;
	}
	else
	{
		return mTargetPos;
	}
}

/**
*	\brief �����ƶ��ٶȣ�������
*/
void TargetMovement::SetMovingSpeed(int movingSpeed)
{
	mMovingSpeed = movingSpeed;
	RefreshMovment();
}

int TargetMovement::GetMovingSpeed() const
{
	return mMovingSpeed;
}

/**
*	\brief ˢ�µ�ǰmovement״̬
*
*	�������¼����ƶ�����,�����ٶȷ���
*/
void TargetMovement::RefreshMovment()
{
	if (mTargetEntity != nullptr)
	{
		mTargetPos = mTargetEntity->GetPos() + mTargetOffset;
	}
	if (this->GetPos() != mTargetPos)
	{
		mFinished = false;
		Point2 dxy = mTargetPos - GetPos();
		mSignX = dxy.x >= 0 ? 1 : -1;
		mSignY = dxy.y >= 0 ? 1 : -1;

		double angle = Geometry::GetAngle(GetPos(), mTargetPos);

		// ���Ƕȱ仯�㹻������ٶ�̫С
		if (std::abs(angle - GetAngle()) > 1E-5 || GetSpeed() < 1E-5)
		{
			SetSpeed(mMovingSpeed);
			SetAngle(angle);
			SetMaxDistance(Geometry::GetDistance(GetPos(), mTargetPos));
		}
	}

}

void TargetMovement::Start()
{
	mFinished = false;
	RefreshPos();
	RefreshMovment();
	Movement::Start();
}

/**
*	\brief ÿ֡ˢ�²���
*/
void TargetMovement::Update()
{
	if (System::Now() >= mNextRefreshMovmentDate)
	{
		RefreshMovment();
		mNextRefreshMovmentDate += refreshMovementDelay;
	}	

	// ���˶�����ƫ��ʱ����Ϊtargetλ��
	Point2 dxy = mTargetPos - GetPos();
	if ( dxy.x * mSignX <= 0 && dxy.y * mSignY <= 0)
	{
		SetPos(mTargetPos);
		Stop();
		mFinished = true;
	}

	StraightMovement::Update();
}

bool TargetMovement::IsFinished() const
{
	return mFinished;
}

const string TargetMovement::GetLuaObjectName() const
{
	return LuaContext::module_target_movement_name;
}
