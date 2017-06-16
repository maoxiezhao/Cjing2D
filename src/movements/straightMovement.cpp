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

/**
*	\brief 暂停当前移动
*
*	需要更新当前的nextMoveDate来保证下次的移动正确
*/
void StraightMovement::SetSuspended(bool suspended)
{
	Movement::SetSuspended(suspended);
	if (!suspended)
	{
		if (GetWhenSuspeneded() != 0)
		{
			uint32_t diff = System::Now() - GetWhenSuspeneded();
			mNextMoveDateX += diff;
			mNextMoveDateY += diff;
		}
	}
}

/**
*	\brief 设置当前运动方向
*/
void StraightMovement::SetAngle(double angle)
{
	if (!IsStop())
	{
		double speed = GetSpeed();
		SetSpeedX(speed * cosf(angle));
		SetSpeedY(speed * -sinf(angle));
	}
	mAngle = angle;
	NotifyMovementChanged();
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

/**
*	\brief 设置x方向速度
*/
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

/**
*	\brief 设置y方向速度
*/
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

/**
*	\brief 获取当前运动的矢量速度
*/
double StraightMovement::GetSpeed() const
{
	return sqrtf(mSpeedX * mSpeedX + mSpeedY * mSpeedY);
}

/**
*	\brief 是否开始了运动
*/
bool StraightMovement::IsStarted() const
{
	return mSpeedX != 0 || mSpeedY != 0;
}

bool StraightMovement::IsFinished() const
{
	return mFinished;
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
	if (IsSuspended())
	{
		uint32_t delay = date - System::Now();
		mNextMoveDateX = GetWhenSuspeneded() + delay;
	}
	else
	{
		mNextMoveDateX = date;
	}
}

void StraightMovement::SetNextMoveDateY(uint32_t date)
{
	if (IsSuspended())
	{
		uint32_t delay = date - System::Now();
		mNextMoveDateY = GetWhenSuspeneded() + delay;
	}
	else
	{
		mNextMoveDateY = date;
	}
}

const string StraightMovement::GetLuaObjectName() const
{
	return string();
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
				Geometry::GetDistance(mStartedPoint, GetPos()) >= mMaxDistance)
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

/**
*	\brief 更新xy方向坐标，由update调用
*/
void StraightMovement::UpdateXY()
{
	Point2 mOldPos = GetPos();

	uint32_t now = System::Now();
	bool isMoveX = mDirectionX != 0 && now > mNextMoveDateX;
	bool isMoveY = mDirectionY != 0 && now > mNextMoveDateY;

	if (isMoveX)
	{
		if (isMoveY)
		{
			// 同时移动x,y方向
			if (!TestCollisionWithObstacles(Point2(mDirectionX, mDirectionY)))
			{
				TranslatePos(Point2(mDirectionX, mDirectionY));
			}
			mNextMoveDateX += mDelayX;
			mNextMoveDateY += mDelayY;
		}
		else
		{
			// 移动x方向
			if (!TestCollisionWithObstacles(Point2(mDirectionX, 0)))
			{
				TranslatePos(Point2(mDirectionX, 0));
			}
			mNextMoveDateX += mDelayX;
		}
	}
	else
	{
		// 移动y方向
		if (!TestCollisionWithObstacles(Point2(0,mDirectionY)))
		{
			TranslatePos(Point2(0,mDirectionY));
		}
		mNextMoveDateY += mDelayY;
	}

	if (!mFinished)
	{
		// 判断是否移动成功，如果移动失败，则触及障碍物
		bool successs = (GetPos() != mOldPos) && (mDirectionX != 0 || mDirectionY != 0);
		if (!successs)
		{
			NotifyObstacleReached();
		}
	}

}

void StraightMovement::UpdateSmoothX()
{
}

void StraightMovement::UpdateSmoothY()
{
}
