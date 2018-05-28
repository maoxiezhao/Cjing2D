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
		SetSpeedX(speed * cos(angle));
		SetSpeedY(speed * -sin(angle));
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
	SetSpeedY(speed * -std::sin(mAngle));	// 原点在左上角
	mAngle = oldAngle;
	mSpeedValue = speed;
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
	mSpeedValue = speed.GetLengthSquared();
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
*	\brief 设置y方向速度
*/
void StraightMovement::SetSpeedY(double speedY )
{
	// 速度过小，则忽略
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
*	\brief 获取当前运动的矢量速度
*/
double StraightMovement::GetSpeed() const
{
	return sqrt(mSpeedX * mSpeedX + mSpeedY * mSpeedY);
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
			// 如果超过最大移动距离，则结束移动
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
*	\brief 更新xy方向坐标，由update调用
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

/**
*	\brief 更流畅的移动方式？？
*
*	相比普通的update会存在更多的碰撞检测，性能相比普通的update
*	差很多，同时解决了撞墙斜向不移动问题
*/
void StraightMovement::UpdateSmoothXY()
{
	Point2 mOldPos = GetPos();

	uint32_t now = System::Now();
	bool isMoveX = mDirectionX != 0 && now >= mNextMoveDateX;
	bool isMoveY = mDirectionY != 0 && now >= mNextMoveDateY;

	// 这里和普通的updateXY存在不同，当xy方向同时可以移动时
	// 这里依然会根据先后顺序移动2个方向,单方向时和普通updte
	// 相同，但是会使用更自然的移动updateSmooth
	if (isMoveX)
	{
		if (isMoveY)
		{
			// 这里依然会根据先后顺序移动2个方向，避免无法移动
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
		// 判断是否移动成功，如果移动失败，则触及障碍物
		bool successs = (GetPos() != mOldPos) && (mDirectionX != 0 || mDirectionY != 0);
		if (!successs)
		{
			NotifyObstacleReached();
		}
	}
}

/**
*	\brief 平缓的在x方向移动
*
*	该函数会检测y方向是否可以移动，如果不能移动
*	则以speed的速度在x方向移动
*/
void StraightMovement::UpdateSmoothX()
{
	if (mDirectionX != 0)
	{
		uint32_t nextMoveDateIncrease = mDelayX;
		// 移动x方向
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
			// 在x方向无法移动，但是可能会存在entity卡在墙角的可能
			// 此时给一个y方向移动
			if (mDirectionY == 0)
			{	
				// 为了使拐角处的移动更顺畅，这里最大检测八个像素
				// 如果8个像素之内可以移动，则给一个y方向移动
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
*	\brief 平缓的在x方向移动
*
*	该函数会检测x方向是否可以移动，如果不能移动
*	则以speed的速度在y方向移动
*/
void StraightMovement::UpdateSmoothY()
{
	if (mDirectionY != 0)
	{
		uint32_t nextMoveDateIncrease = mDelayY;
		// 移动x方向
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
			// 在x方向无法移动，但是可能会存在entity卡在墙角的可能
			// 此时给一个y方向移动
			if (mDirectionX == 0)
			{
				// 为了使拐角处的移动更顺畅，这里最大检测八个像素
				// 如果8个像素之内可以移动，则给一个y方向移动
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
