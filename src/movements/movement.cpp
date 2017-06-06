#include "movement.h"
#include "core\system.h"
#include "lua\luaContext.h"


Movement::Movement():
	mPos(Point2(0,0)),
	mFinished(false),
	mSuspended(false),
	mDrawable(nullptr),
	mWhenSuspendedTime(0),
	mIngoreObstacle(false)
{
}

Movement::Movement(bool ingoreObstacles):
	mPos(Point2(0, 0)),
	mFinished(false),
	mSuspended(false),
	mDrawable(nullptr),
	mWhenSuspendedTime(0),
	mIngoreObstacle(ingoreObstacles)
{
}

Movement::~Movement()
{
}

void Movement::Update()
{
	if (!mFinished && IsFinished() )
	{
		mFinished = true;
		NotifyMovementFinished();
	}
	else if (mFinished && !IsFinished())
	{
		mFinished = false;
	}
}

bool Movement::IsSuspended() const
{
	return false;
}

void Movement::SetSuspended(bool suspended)
{
	if (suspended != mSuspended)
	{
		mSuspended = suspended;
		if (suspended)
		{
			mWhenSuspendedTime = System::Now();
		}
	}
}

/**
*	\brief 移动是否完成
*
*	派生类可以重载是否完成的条件判断
*/
bool Movement::IsFinished() const
{
	return mFinished;
}

/**
*	\brief 响应移动完成
*/
void Movement::NotifyMovementFinished()
{
	LuaContext* luaContext = GetLuaContext();
	if (luaContext != nullptr )
	{
		mFinishedCallBack.Call("movment finished callback");
	}
}

const string Movement::GetLuaObjectName() const
{
	return LuaContext::module_movement_name;
}

bool Movement::TestCollisionWithObstacles(const Point2 & dxy) const
{
	return false;
}

bool Movement::IsIngoredObstacles() const
{
	return mIngoreObstacle;
}

void Movement::SetIngoredObstacles(bool ingoredObstacles)
{
	mIngoreObstacle = ingoredObstacles;
}

void Movement::Stop()
{
	mFinished = true;
}

Point2 Movement::GetPos() const
{
	return mPos;;
}

void Movement::SetX(const int x)
{
	mPos.x = x;
}

void Movement::SetY(const int y)
{
	mPos.y = y;
}

void Movement::SetPos(const Point2 & pos)
{
	mPos = pos;
}

void Movement::TranslateX(int x)
{
	mPos.x += x;
}

void Movement::TranslateY(int y)
{
	mPos.y += y;
}

void Movement::TranslatePos(const Point2 & dxy)
{
	mPos += dxy;
}