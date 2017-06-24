#include "movement.h"
#include "game\drawable.h"
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

/**
*	\brief 每帧刷新事件
*
*    movement基类的update会在派生类调用，这里主要对是否finished进行处理
*/
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
*	\brief 响应移动变换
*/
void Movement::NotifyMovementChanged()
{
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

void Movement::NotifyObstacleReached()
{

}

const string Movement::GetLuaObjectName() const
{
	return LuaContext::module_movement_name;
}

/**
*	\brief 测试是否发生了碰撞
*	\param dxy 相对当前位置的偏移值
*	\return 无发生碰撞返回False,发生了碰撞返回true
*/
bool Movement::TestCollisionWithObstacles(const Point2 & dxy) const
{
	if (mIngoreObstacle || mEntity == nullptr)
	{
		return false;
	}

	//Map map = GetMap();
	//Rect rect = GetBoundsRect();
	//rect.translate(dxy);

	//bool collision = map->TestCollisionWithObstacles(rect, *mEntity);

	bool collision = false;
	
	return collision;
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

bool Movement::IsStop() const
{
	return !IsStarted();
}

bool Movement::IsStarted() const
{
	return false;
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

/**
*	\brief 获取暂定时的时间
*/
uint32_t Movement::GetWhenSuspeneded() const
{
	return mWhenSuspendedTime;
}

/**
*	\brief 设置当前移动的drwable宿主
*/
void Movement::SetDrawable(Drawable * drawable)
{
	mDrawable = drawable;
	if (mDrawable == nullptr)
	{
		mPos = {0, 0};
	}
	else
	{
		mPos = mDrawable->GetPos();
		NotifyMovementChanged();
	}
}
