#include "straightMovement.h"
#include "core\system.h"
#include "utils\geometry.h"

StraightMovement::StraightMovement():
	Movement(),
	mSpeedX(0.0f),
	mSpeedY(0.0f),
	mAngle(0.0f),
	mNextMoveDateX(0),
	mNextMoveDateY(0),
	mDelayX(0),
	mDelayY(0),
	mMaxDistance(0),
	mFinished(false)
{
}

StraightMovement::StraightMovement(bool isIngroedObstacles):
	Movement(isIngroedObstacles),
	mSpeedX(0.0f),
	mSpeedY(0.0f),
	mAngle(0.0f),
	mNextMoveDateX(0),
	mNextMoveDateY(0),
	mDelayX(0),
	mDelayY(0),
	mMaxDistance(0),
	mFinished(false)
{
}

void StraightMovement::Update()
{
	if (!IsSuspended())
	{
		uint32_t now = System::Now();
		bool isMoveX = mDirectionX != 0 && now > mNextMoveDateX;
		bool isMoveY = mDirectionY != 0 && now > mNextMoveDateY;

		while (isMoveX || isMoveY)
		{
			UpdateXY();
			now = System::Now();

			// 如果超过最大移动距离，则结束移动
			if (!mFinished && 
				mMaxDistance != 0 &&
				Geometry::GetDistance(mStartedPoint,GetPos()) >= mMaxDistance )
			{
				SetFinished(true);
			}
			else
			{
				bool isMoveX = mDirectionX != 0 && now > mNextMoveDateX;
				bool isMoveY = mDirectionY != 0 && now > mNextMoveDateY;
			}
		}
	}

	Movement::Update();
}

void StraightMovement::SetSuspended(bool suspended)
{
}

void StraightMovement::SetAngle(double angle)
{
}

/**
*	\brief 设置当前速度
*	\param speed 速度标量
*
*	会根据当前angle设置xy速度分量
*/
void StraightMovement::SetSpeed(double speed)
{
	double oldAngle = this->mAngle;
	SetSpeedX(speed * std::cos(mAngle));
	SetSpeedY(speed * -std::cos(mAngle));	// 原点在左上角
	this->mAngle = oldAngle;

	NotifyMovementChanged();
}

/**
*	\brief 设置当前速度
*	\param speed vec2f速度向量，同时会设置angle
*/
void StraightMovement::SetSpeedVec2(const Vec2f & speed)
{
	SetSpeedX(speed.x);
	SetSpeedY(speed.y);

	NotifyMovementChanged();
}

void StraightMovement::SetSpeedX(double speedX )
{
	// 速度过小，则忽略
	if (std::fabs(speedX) <= 1E-5)
	{
		speedX = 0;
	}

	uint32_t now = System::Now();
	if (speedX == 0)
	{
		mDirectionX = 0;
	}
	else 
	{
		if (speedX > 0)
		{
			mDelayX = (uint32_t)(1000 / speedX);
			mDirectionX = 1;
		}
		else
		{
			mDelayX = (uint32_t)(1000 / -speedX);
			mDirectionX = -1;
		}
		SetNextMoveDateX(now + mDelayX);
	}
	this->mAngle = Geometry::GetAngle(0.0f, 0.0f, mSpeedX * 1000, mSpeedY * 1000);
	mStartedPoint = GetPos();
	mFinished = false;

	NotifyMovementChanged();
}

void StraightMovement::SetSpeedY(double speedY )
{
	// 速度过小，则忽略
	if (std::fabs(speedY) <= 1E-5)
	{
		speedY = 0;
	}

	uint32_t now = System::Now();
	if (speedY == 0)
	{
		mDirectionY = 0;
	}
	else
	{
		if (speedY > 0)
		{
			mDelayY = (uint32_t)(1000 / speedY);
			mDirectionY = 1;
		}
		else
		{
			mDelayY = (uint32_t)(1000 / -speedY);
			mDirectionY = -1;
		}
		SetNextMoveDateY(now + mDelayY);
	}
	this->mAngle = Geometry::GetAngle(0.0f, 0.0f, mSpeedX * 1000, mSpeedY * 1000);
	mStartedPoint = GetPos();
	mFinished = false;

	NotifyMovementChanged();
}

bool StraightMovement::IsFinished() const
{
	return false;
}

/**
*	\brief 停止当前动作
*/
void StraightMovement::Stop()
{
	double oldAngle = this->mAngle;
	SetSpeedX(0);
	SetSpeedY(0);
	mDirectionX = 0;
	mDirectionY = 0;
	this->mAngle = oldAngle;
	NotifyMovementChanged();
}

void StraightMovement::SetFinished(bool finished)
{
	Stop();
	mFinished = true;
}

void StraightMovement::SetNextMoveDateX(uint32_t date)
{
}

void StraightMovement::SetNextMoveDateY(uint32_t date)
{
}

const string StraightMovement::GetLuaObjectName() const
{
	return string();
}

void StraightMovement::UpdateXY()
{
}

void StraightMovement::UpdateSmoothX()
{
}

void StraightMovement::UpdateSmoothY()
{
}
