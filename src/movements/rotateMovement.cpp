#include"rotateMovement.h"
#include"core\system.h"
#include"lua\luaContext.h"

RotateMovement::RotateMovement():
	Movement(),
	mRotateSpeed(0.0f),
	mAngleAmount(0.0f),
	mCurAngle(0.0f),
	mFinished(false)
{
}

RotateMovement::RotateMovement(float angleAmount, float rotateSpeed):
	Movement(),
	mRotateSpeed(rotateSpeed),
	mAngleAmount(angleAmount),
	mCurAngle(0.0f),
	mFinished(false)
{
	SetRotateSpeed(rotateSpeed);
}

void RotateMovement::Update()
{
	if (!IsSuspended())
	{
		uint32_t now = System::Now();
		bool isRotate = mRotateDirection != 0 && now >= mNextRotateDate;

		while (isRotate)
		{
			TranslateRotate((float)mRotateDirection);
			mNextRotateDate += mRotateDelay;
			mCurAngle += fabsf(mRotateDirection);

			now = System::Now();
			if (mAngleAmount != 0 && mCurAngle >= mAngleAmount)
			{
				SetFinished(true);
			}
			else
			{
				isRotate = mRotateDirection != 0 && now >= mNextRotateDate;
			}
		}
	}
	Movement::Update();
}

void RotateMovement::SetSuspended(bool suspended)
{
	Movement::SetSuspended(suspended);
	if (!suspended)
	{
		if (GetWhenSuspeneded() != 0)
		{
			uint32_t diff = System::Now() - GetWhenSuspeneded();
			mNextRotateDate += diff;
		}
	}
}

void RotateMovement::SetRotateSpeed(float speed)
{
	// 速度过小，则忽略
	if (std::fabs(speed) <= 1E-5)
		mRotateSpeed = 0;
	
	mRotateSpeed = speed;
	if (mRotateSpeed == 0)
	{
		mRotateDirection = 0;
	}
	else
	{
		if (speed > 0)
		{
			mRotateDelay = (uint32_t)(1000.0f / speed);
			mRotateDirection = 1.0f;
		}
		else
		{
			mRotateDelay = (uint32_t)(1000.0f / -speed);
			mRotateDirection = -1.0f;
		}
		mNextRotateDate = System::Now() + mRotateDelay;
	}
	mCurAngle = 0.0f;
	mFinished = false;

	NotifyMovementChanged();
}

float RotateMovement::GetRotateSpeed() const
{
	return mRotateSpeed;
}

void RotateMovement::SetAngleAmount(float amount)
{
	mAngleAmount = amount;
}

bool RotateMovement::IsFinished() const
{
	return mFinished;
}

void RotateMovement::Stop()
{
	mRotateDirection = 0;
	mRotateSpeed = 0;
	mAngleAmount = 0;
	mCurAngle = 0;

	NotifyMovementChanged();
}

void RotateMovement::SetFinished(bool finished)
{
	Stop();
	mFinished = true;
}

const string RotateMovement::GetLuaObjectName() const
{
	return LuaContext::module_rotate_movement_name;
}

