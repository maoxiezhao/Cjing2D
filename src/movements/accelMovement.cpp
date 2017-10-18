#include "accelMovement.h"
#include "core\system.h"
#include "utils\geometry.h"

AccelMovement::AccelMovement():
	Movement(false),
	mRadialAccel(0.0),
	mTangentialAccel(0.0),
	mGravityAccel(0.0),
	mSpeed(0, 0),
	mAngle(0),
	mDistanceX(0),
	mDistanceY(0)
{
}

AccelMovement::AccelMovement(double radialAccel, double tangentialAccel, double gravityAccel, bool ingoreObstacles):
	Movement(ingoreObstacles),
	mRadialAccel(radialAccel),
	mTangentialAccel(tangentialAccel),
	mGravityAccel(gravityAccel),
	mSpeed(0, 0),
	mAngle(0),
	mDistanceX(0),
	mDistanceY(0)
{
}

/**
*	\brief 更新当前movement
*/
void AccelMovement::Update()
{
	if (!IsSuspended() && !IsFinished())
	{
		UpdateSpeed();

		if (mSpeed.x != 0 || mSpeed.y != 0)
		{
			UpdateXY();
		}
	}
	Movement::Update();
}

bool AccelMovement::IsStarted() const
{
	return  mSpeed.x != 0 ||
			mSpeed.y != 0 ||
			mRadialAccel != 0 ||
			mTangentialAccel != 0 ||
			mGravityAccel != 0;
}

void AccelMovement::SetRadialAccel(double radialAccel)
{
	mRadialAccel = radialAccel;
	NotifyMovementChanged();
}

void AccelMovement::SetTangentialAccel(double tangentialAccel)
{
	mTangentialAccel = mTangentialAccel;
	NotifyMovementChanged();
}

void AccelMovement::SetGravityAccel(double gravityAccel)
{
	mGravityAccel = gravityAccel;
	NotifyMovementChanged();
}

/**
*	\brief 设置速度的方向
*	\param angle 速度角度的弧度
*/
void AccelMovement::SetAngle(double angle)
{
	double speed = GetSpeedVar();
	mSpeed.x = (speed * cos(angle));
	mSpeed.y = (speed * -sin(angle));
	mAngle = angle;
	
	NotifyMovementChanged();
}

/**
*	\brief 设置当前速度
*	\param speed 速度的标量
*/
void AccelMovement::SetSpeed(double speed)
{
	mSpeed.x = (speed * cos(mAngle));
	mSpeed.y = (speed * -sin(mAngle));

	NotifyMovementChanged();
}

/**
*	\brief 获取当前速度的标量
*/
double AccelMovement::GetSpeedVar() const
{
	return mSpeed.GetLength();
}

Vec2d AccelMovement::GetSpeed() const
{
	return mSpeed;
}

void AccelMovement::SetOrigin(const Point2 & pos)
{
	mOrigin = pos;
}

/**
*	\brief 更新当前速度
*/
void AccelMovement::UpdateSpeed()
{
	Vec2d radial;
	if (GetPosX() == mOrigin.x &&
		GetPosY() == mOrigin.y)	// 在原点
	{
		radial = Vec2d(0, 0);
	}
	else
	{
		radial = Vec2d(GetPosX() - mOrigin.x,
			GetPosY() - mOrigin.y).Normalized();
	}
	Vec2d tangential = { radial.y, -radial.x };

	// 计算合力加速度
	radial.x *= mRadialAccel;
	radial.y *= mRadialAccel;
	tangential.x *= mTangentialAccel;
	tangential.y *= mTangentialAccel;
	Vec2d finalAccel = radial + tangential + Vec2d(0.0, mGravityAccel);

	mSpeed += finalAccel;
}

/**
*	\brief 更新xy方向坐标，由update调用
*/
void AccelMovement::UpdateXY()
{
	Point2 mOldPos = GetPos();

	// 计算位移量
	mDistanceX += mSpeed.x / 1000 * System::timeStep;
	mDistanceY += mSpeed.y / 1000 * System::timeStep;
	int moveX = (int)(mDistanceX);
	int moveY = (int)(mDistanceY);

	bool isMoveX = moveX != 0;
	bool isMoveY = moveY != 0;
	if (isMoveX)
	{
		if (isMoveY)
		{
			// 同时移动x,y方向
			if (!TestCollisionWithObstacles(Point2(moveX, moveY)))
			{
				TranslatePos(Point2(moveX, moveY));
				mDistanceX -= moveX;
				mDistanceY -= moveY;
			}
		}
		else
		{
			// 移动x方向
			if (!TestCollisionWithObstacles(Point2(moveX, 0)))
			{
				TranslatePos(Point2(moveX, 0));
				mDistanceX -= moveX;
			}
		}
	}
	else
	{
		// 移动y方向
		if (isMoveY)
		{
			if (!TestCollisionWithObstacles(Point2(0, moveY)))
			{
				TranslatePos(Point2(0, moveY));
				mDistanceY -= moveY;
			}
		}
	}

	if (!IsFinished())
	{
		// 判断是否移动成功，如果移动失败，则触及障碍物
		bool successs = (GetPos() != mOldPos) && (moveX != 0 || moveY != 0);
		if (!successs)
		{
			NotifyObstacleReached();
		}
	}
}

