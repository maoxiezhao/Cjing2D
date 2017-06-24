#include "targetMovement.h"
#include "core\system.h"
#include "utils\geometry.h"

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

/**
*	\brief 设置移动到达的目标
*	\param entity 到达的目标的位置为实体位置，此时pos为相对目标的偏移，
				  如果entity为空，pos为到达的位置
*	\param pos  位置或偏移量
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
*	\brief 返回目标位置,目标位置如果是实体则可能会变动
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
*	\brief 设置移动速度（标量）
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
*	\brief 刷新当前movement状态
*
*	包括重新计算移动方向,设置速度分量
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

		// 当角度变化足够大或者速度太小
		if (std::abs(angle - GetAngle()) > 1E-5 || GetSpeed() < 1E-5)
		{
			SetSpeed(mMovingSpeed);
			SetAngle(angle);
			SetMaxDistance(Geometry::GetDistance(GetPos(), mTargetPos));
		}
	}

}

/**
*	\brief 每帧刷新操作
*/
void TargetMovement::Update()
{
	if (System::Now() >= mNextRefreshMovmentDate)
	{
		RefreshMovment();
		mNextRefreshMovmentDate += refreshMovementDelay;
	}	

	// 当运动结束偏移时设置为target位置
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
	return string();
}
