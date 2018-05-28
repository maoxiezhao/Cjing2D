#include "straightMovement.h"
#include "core\system.h"
#include "utils\geometry.h"
#include "lua\luaContext.h"

StraightMovement::StraightMovement():
	Movement(),
	mSpeedX(0.0f),
	mSpeedY(0.0f),
	mSpeedValue(0.0f),
	mAngle(0.0f),
	mNextMoveDateX(0),
	mNextMoveDateY(0),
	mDirectionX(0),
	mDirectionY(0),
	mDelayX(0),
	mDelayY(0),
	mMaxDistance(0),
	mFinished(false),
	mMoveSmooth(false)
{
}

StraightMovement::StraightMovement(bool isIngroedObstacles, bool smooth):
	Movement(isIngroedObstacles),
	mSpeedX(0.0f),
	mSpeedY(0.0f),
	mSpeedValue(0.0f),
	mAngle(0.0f),
	mNextMoveDateX(0),
	mNextMoveDateY(0),
	mDirectionX(0),
	mDirectionY(0),
	mDelayX(0),
	mDelayY(0),
	mMaxDistance(0),
	mFinished(false),
	mMoveSmooth(smooth)
{
}

StraightMovement::~StraightMovement()
{
	Movement::~Movement();
	mFinished = true;
}

/**
*	\brief ��ͣ��ǰ�ƶ�
*
*	��Ҫ���µ�ǰ��nextMoveDate����֤�´ε��ƶ���ȷ
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
*	\brief ���õ�ǰ�˶�����
*/
void StraightMovement::SetAngle(double angle)
{
	if (!IsStop())
	{
		double speed = GetSpeed();
		SetSpeedX(speed * cos(angle));
		SetSpeedY(speed * -sin(angle));
	}
	mAngle = angle;
	NotifyMovementChanged();
}

/**
*	\brief ���õ�ǰ�ٶ�
*	\param speed �ٶȱ���
*
*	����ݵ�ǰangle����xy�ٶȷ���
*/
void StraightMovement::SetSpeed(double speed)
{
	double oldAngle = this->mAngle;
	SetSpeedX(speed * std::cos(mAngle));
	SetSpeedY(speed * -std::sin(mAngle));	// ԭ�������Ͻ�
	mAngle = oldAngle;
	mSpeedValue = speed;
	NotifyMovementChanged();
}

/**
*	\brief ���õ�ǰ�ٶ�
*	\param speed vec2f�ٶ�������ͬʱ������angle
*/
void StraightMovement::SetSpeedVec2(const Vec2f & speed)
{
	SetSpeedX(speed.x);
	SetSpeedY(speed.y);
	mSpeedValue = speed.GetLengthSquared();
	NotifyMovementChanged();
}

/**
*	\brief ����x�����ٶ�
*/
void StraightMovement::SetSpeedX(double speedX )
{
	// �ٶȹ�С�������
	if (std::fabs(speedX) <= 1E-5)
	{
		speedX = 0;
	}
	mSpeedX = speedX;
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
*	\brief ����y�����ٶ�
*/
void StraightMovement::SetSpeedY(double speedY )
{
	// �ٶȹ�С�������
	if (std::fabs(speedY) <= 1E-5)
	{
		speedY = 0;
	}
	mSpeedY = speedY;
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

void StraightMovement::SetMaxDistance(double distance)
{
	mMaxDistance = distance;
}

double StraightMovement::GetMaxDistance() const
{
	return mMaxDistance;
}

double StraightMovement::GetAngle() const
{
	return mAngle;
}

/**
*	\brief ��ȡ��ǰ�˶���ʸ���ٶ�
*/
double StraightMovement::GetSpeed() const
{
	return sqrt(mSpeedX * mSpeedX + mSpeedY * mSpeedY);
}

/**
*	\brief �Ƿ�ʼ���˶�
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
*	\brief ֹͣ��ǰ����
*/
void StraightMovement::Stop()
{
	double oldAngle = this->mAngle;
	SetSpeedX(0);
	SetSpeedY(0);
	SetMaxDistance(0);
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
	return LuaContext::module_straight_movement_name;
}

void StraightMovement::Update()
{
	if (!IsSuspended())
	{
		uint32_t now = System::Now();
		bool isMoveX = mDirectionX != 0 && now >= mNextMoveDateX;
		bool isMoveY = mDirectionY != 0 && now >= mNextMoveDateY;

		while (isMoveX || isMoveY)
		{
			if(mMoveSmooth)
				UpdateSmoothXY();
			else
				UpdateXY();

			now = System::Now();
			// �����������ƶ����룬������ƶ�
			if (!mFinished &&
				mMaxDistance != 0 &&
				Geometry::GetDistance(mStartedPoint, GetPos()) >= mMaxDistance)
			{
				SetFinished(true);
			}
			else
			{
				isMoveX = (mDirectionX != 0 && now >= mNextMoveDateX);
				isMoveY = (mDirectionY != 0 && now >= mNextMoveDateY);
			}
		}
	}

	Movement::Update();
}

/**
*	\brief ����xy�������꣬��update����
*/
void StraightMovement::UpdateXY()
{
	Point2 mOldPos = GetPos();

	uint32_t now = System::Now();
	bool isMoveX = mDirectionX != 0 && now >= mNextMoveDateX;
	bool isMoveY = mDirectionY != 0 && now >= mNextMoveDateY;

	if (isMoveX)
	{
		if (isMoveY)
		{
			// ͬʱ�ƶ�x,y����
			if (!TestCollisionWithObstacles(Point2(mDirectionX, mDirectionY)))
			{
				TranslatePos(Point2(mDirectionX, mDirectionY));
			}
			mNextMoveDateX += mDelayX;
			mNextMoveDateY += mDelayY;
		}
		else
		{
			// �ƶ�x����
			if (!TestCollisionWithObstacles(Point2(mDirectionX, 0)))
			{
				TranslatePos(Point2(mDirectionX, 0));
			}
			mNextMoveDateX += mDelayX;
		}
	}
	else
	{
		// �ƶ�y����
		if (!TestCollisionWithObstacles(Point2(0,mDirectionY)))
		{
			TranslatePos(Point2(0,mDirectionY));
		}
		mNextMoveDateY += mDelayY;
	}

	if (!mFinished)
	{
		// �ж��Ƿ��ƶ��ɹ�������ƶ�ʧ�ܣ��򴥼��ϰ���
		bool successs = (GetPos() != mOldPos) && (mDirectionX != 0 || mDirectionY != 0);
		if (!successs)
		{
			NotifyObstacleReached();
		}
	}

}

/**
*	\brief ���������ƶ���ʽ����
*
*	�����ͨ��update����ڸ������ײ��⣬���������ͨ��update
*	��ܶ࣬ͬʱ�����ײǽб���ƶ�����
*/
void StraightMovement::UpdateSmoothXY()
{
	Point2 mOldPos = GetPos();

	uint32_t now = System::Now();
	bool isMoveX = mDirectionX != 0 && now >= mNextMoveDateX;
	bool isMoveY = mDirectionY != 0 && now >= mNextMoveDateY;

	// �������ͨ��updateXY���ڲ�ͬ����xy����ͬʱ�����ƶ�ʱ
	// ������Ȼ������Ⱥ�˳���ƶ�2������,������ʱ����ͨupdte
	// ��ͬ�����ǻ�ʹ�ø���Ȼ���ƶ�updateSmooth
	if (isMoveX)
	{
		if (isMoveY)
		{
			// ������Ȼ������Ⱥ�˳���ƶ�2�����򣬱����޷��ƶ�
			if (mNextMoveDateX <= mNextMoveDateY)
			{
				UpdateSmoothX();
				if (now >= mNextMoveDateY)
					UpdateSmoothY();
			}
			else
			{
				UpdateSmoothY();
				if (now >= mNextMoveDateX)
					UpdateSmoothX();
			}
		}
		else
		{
			UpdateSmoothX();
		}
	}
	else
	{
		UpdateSmoothY();
	}

	if (!mFinished)
	{
		// �ж��Ƿ��ƶ��ɹ�������ƶ�ʧ�ܣ��򴥼��ϰ���
		bool successs = (GetPos() != mOldPos) && (mDirectionX != 0 || mDirectionY != 0);
		if (!successs)
		{
			NotifyObstacleReached();
		}
	}
}

/**
*	\brief ƽ������x�����ƶ�
*
*	�ú�������y�����Ƿ�����ƶ�����������ƶ�
*	����speed���ٶ���x�����ƶ�
*/
void StraightMovement::UpdateSmoothX()
{
	if (mDirectionX != 0)
	{
		uint32_t nextMoveDateIncrease = mDelayX;
		// �ƶ�x����
		if (!TestCollisionWithObstacles({ mDirectionX, 0 }))
		{
			TranslatePos(Point2(mDirectionX, 0));

			if (mDirectionY != 0 && TestCollisionWithObstacles({ 0, mDirectionY }))
			{
				nextMoveDateIncrease = (uint32_t)(1000 / mSpeedValue);
			}
		}
		else
		{
			// ��x�����޷��ƶ������ǿ��ܻ����entity����ǽ�ǵĿ���
			// ��ʱ��һ��y�����ƶ�
			if (mDirectionY == 0)
			{	
				// Ϊ��ʹ�սǴ����ƶ���˳�������������˸�����
				// ���8������֮�ڿ����ƶ������һ��y�����ƶ�
				bool bMoved = false;
				for (int i = 1; i <= 8 && !bMoved; i++)
				{
					if (!TestCollisionWithObstacles({ mDirectionX, i }) &&
						!TestCollisionWithObstacles({0, 1}) )
					{
						TranslatePos(Point2(0, 1));
						bMoved = true;
					}
					else if (!TestCollisionWithObstacles({ mDirectionX, -i }) &&
						!TestCollisionWithObstacles({ 0, -1 }))
					{
						TranslatePos(Point2(0, -1));
						bMoved = true;
					}
				}

			}
		}
		mNextMoveDateX += nextMoveDateIncrease;
	}
}

/**
*	\brief ƽ������x�����ƶ�
*
*	�ú�������x�����Ƿ�����ƶ�����������ƶ�
*	����speed���ٶ���y�����ƶ�
*/
void StraightMovement::UpdateSmoothY()
{
	if (mDirectionY != 0)
	{
		uint32_t nextMoveDateIncrease = mDelayY;
		// �ƶ�x����
		if (!TestCollisionWithObstacles(Point2(0, mDirectionY)))
		{
			TranslatePos(Point2(0, mDirectionY));

			if (mDirectionY != 0 && TestCollisionWithObstacles(Point2(mDirectionX, 0)))
			{
				nextMoveDateIncrease = (uint32_t)(1000 / mSpeedValue);
			}
		}
		else
		{
			// ��x�����޷��ƶ������ǿ��ܻ����entity����ǽ�ǵĿ���
			// ��ʱ��һ��y�����ƶ�
			if (mDirectionX == 0)
			{
				// Ϊ��ʹ�սǴ����ƶ���˳�������������˸�����
				// ���8������֮�ڿ����ƶ������һ��y�����ƶ�
				bool bMoved = false;
				for (int i = 0; i <= 8 && !bMoved; i++)
				{
					if (!TestCollisionWithObstacles({ i, mDirectionY }) &&
						!TestCollisionWithObstacles({ 1, 0 }))
					{
						TranslatePos(Point2(1, 0));
						bMoved = true;
					}
					else if (!TestCollisionWithObstacles({ -i, mDirectionY }) &&
						!TestCollisionWithObstacles({ -1, 0 }))
					{
						TranslatePos(Point2(-1, 0));
						bMoved = true;
					}
				}

			}
		}
		mNextMoveDateY += nextMoveDateIncrease;
	}
}
