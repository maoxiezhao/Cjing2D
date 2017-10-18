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
*	\brief ���µ�ǰmovement
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
*	\brief �����ٶȵķ���
*	\param angle �ٶȽǶȵĻ���
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
*	\brief ���õ�ǰ�ٶ�
*	\param speed �ٶȵı���
*/
void AccelMovement::SetSpeed(double speed)
{
	mSpeed.x = (speed * cos(mAngle));
	mSpeed.y = (speed * -sin(mAngle));

	NotifyMovementChanged();
}

/**
*	\brief ��ȡ��ǰ�ٶȵı���
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
*	\brief ���µ�ǰ�ٶ�
*/
void AccelMovement::UpdateSpeed()
{
	Vec2d radial;
	if (GetPosX() == mOrigin.x &&
		GetPosY() == mOrigin.y)	// ��ԭ��
	{
		radial = Vec2d(0, 0);
	}
	else
	{
		radial = Vec2d(GetPosX() - mOrigin.x,
			GetPosY() - mOrigin.y).Normalized();
	}
	Vec2d tangential = { radial.y, -radial.x };

	// ����������ٶ�
	radial.x *= mRadialAccel;
	radial.y *= mRadialAccel;
	tangential.x *= mTangentialAccel;
	tangential.y *= mTangentialAccel;
	Vec2d finalAccel = radial + tangential + Vec2d(0.0, mGravityAccel);

	mSpeed += finalAccel;
}

/**
*	\brief ����xy�������꣬��update����
*/
void AccelMovement::UpdateXY()
{
	Point2 mOldPos = GetPos();

	// ����λ����
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
			// ͬʱ�ƶ�x,y����
			if (!TestCollisionWithObstacles(Point2(moveX, moveY)))
			{
				TranslatePos(Point2(moveX, moveY));
				mDistanceX -= moveX;
				mDistanceY -= moveY;
			}
		}
		else
		{
			// �ƶ�x����
			if (!TestCollisionWithObstacles(Point2(moveX, 0)))
			{
				TranslatePos(Point2(moveX, 0));
				mDistanceX -= moveX;
			}
		}
	}
	else
	{
		// �ƶ�y����
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
		// �ж��Ƿ��ƶ��ɹ�������ƶ�ʧ�ܣ��򴥼��ϰ���
		bool successs = (GetPos() != mOldPos) && (moveX != 0 || moveY != 0);
		if (!successs)
		{
			NotifyObstacleReached();
		}
	}
}

