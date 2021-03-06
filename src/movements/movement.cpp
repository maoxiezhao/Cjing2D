#include "movement.h"
#include "game\drawable.h"
#include "game\map.h"
#include "core\system.h"
#include "lua\luaContext.h"
#include "entity\entity.h"
#include "gui\widget\widget.h"

Movement::Movement():
	mPos(Point2(0,0)),
	mAngle(0.0f),
	mFinished(false),
	mSuspended(false),
	mDrawable(nullptr),
	mEntity(nullptr),
	mWidget(nullptr),
	mWhenSuspendedTime(0),
	mIngoreObstacle(false)
{
}

Movement::Movement(bool ingoreObstacles):
	mPos(Point2(0, 0)),
	mAngle(0.0f),
	mFinished(false),
	mSuspended(false),
	mDrawable(nullptr),
	mEntity(nullptr),
	mWidget(nullptr),
	mWhenSuspendedTime(0),
	mIngoreObstacle(ingoreObstacles)
{
}

Movement::~Movement()
{
	mEntity = nullptr;
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

Entity * Movement::GetEntity()
{
	return mEntity;
}

Drawable * Movement::GetDrawable()
{
	return mDrawable;
}

gui::Widget * Movement::GetWidget()
{
	return mWidget;
}

bool Movement::HasOwner() const
{
	return mEntity != nullptr || 
		   mDrawable != nullptr || 
		   mWidget != nullptr;
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

void Movement::Start()
{
	mFinished = false;
}

/**
*	\brief 响应位置移动
*/
void Movement::NotifyPositonChanged()
{
	if (mEntity != nullptr && !mEntity->IsBeRemoved())
	{
		mEntity->NotifyPositonChanged();
	}
}

/**
*	\brief 响应移动变换
*/
void Movement::NotifyMovementChanged()
{
	if (mEntity != nullptr && !mEntity->IsBeRemoved())
	{
		mEntity->NotifyMovementChanged();
	}
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

	if (mEntity != nullptr && !mEntity->IsBeRemoved())
		mEntity->NotifyMovementFinished();
}

void Movement::NotifyObstacleReached()
{
	if (mEntity != nullptr && !mEntity->IsBeRemoved())
		mEntity->NotifyObstacleReached();
}

const string Movement::GetLuaObjectName() const
{
	return LuaContext::module_movement_name;
}

void Movement::SetFinishedCallBack(const LuaRef & callback)
{
	mFinishedCallBack = callback;
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

	// 获取当前entity的碰撞盒，并偏移位移量后
	// 参与碰撞检测
	Map& map = mEntity->GetMap();
	Rect rect = mEntity->GetRectBounding(Entity::BOUNDING_BOX_OBSTACLE);
	rect.AddPos(dxy.x, dxy.y);

	bool collision = map.TestCollisionWithObstacle(rect, *mEntity);
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

/**
*	\brief 刷新当前位置值
*/
void Movement::RefreshPos()
{
	Point2 curPos;
	if (mEntity != nullptr)
	{
		curPos = mEntity->GetPos();
	}
	if (mDrawable != nullptr)
	{
		curPos = mDrawable->GetPos();
	}
	if (mWidget != nullptr)
	{
		curPos = mWidget->GetPositon();
	}
	if (curPos != GetPos())
	{
		SetPos(curPos);
	}
}

int Movement::GetPosX() const
{
	return mPos.x;
}

int Movement::GetPosY() const
{
	return mPos.y;
}

Point2 Movement::GetPos() const
{
	return mPos;
}

void Movement::SetX(const int x)
{
	SetPos(Point2(x, GetPosY()));
}

void Movement::SetY(const int y)
{
	SetPos(Point2(GetPosX(), y));
}

void Movement::SetPos(const Point2 & pos)
{
	if (mEntity != nullptr)
	{
		mEntity->SetPos(pos);
	}
	if (mDrawable != nullptr)
	{
		mDrawable->SetPos(pos);
	}
	if (mWidget != nullptr)
	{
		mWidget->SetWantedPosition(pos);
	}
	mPos = pos;
	NotifyPositonChanged();
}

void Movement::TranslateX(int x)
{
	TranslatePos(Point2(x, 0));
}

void Movement::TranslateY(int y)
{
	TranslatePos(Point2(0, y));
}

void Movement::TranslatePos(const Point2 & dxy)
{
	SetPos(Point2(GetPosX() + dxy.x, GetPosY() + dxy.y));
}

float Movement::GetAngle() const
{
	return mAngle;
}

void Movement::SetAngle(float angle)
{
	if (mEntity != nullptr)
	{
		mEntity->SetFacingDegree(angle);
	}
	if (mDrawable != nullptr)
	{
		mDrawable->SetRotated(angle);
	}
	if (mWidget != nullptr)
	{
		//
	}
	mAngle = angle;
	NotifyPositonChanged();
}

void Movement::TranslateRotate(float angle)
{
	SetAngle(GetAngle() + angle);
	NotifyPositonChanged();
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
		mAngle = 0.0f;
	}
	else
	{
		mAngle = mDrawable->GetRotated();
		mPos = mDrawable->GetPos();
		NotifyMovementChanged();
	}
}

/**
*	\brief 设置当前移动的entity宿主
*/
void Movement::SetEntity(Entity * entity)
{
	mEntity = entity;
	if (mEntity == nullptr)
	{
		mPos = { 0, 0 };
	}
	else
	{
		mAngle = mEntity->GetFacingDegree();
		mPos = mEntity->GetPos();
		NotifyMovementChanged();
	}
}

void Movement::SetWidget(gui::Widget * widget)
{
	mWidget = widget;
	if (mWidget == nullptr)
	{
		mPos = { 0, 0 };
	}
	else
	{
		mPos = mWidget->GetPositon();
		NotifyMovementChanged();
	}
}

int Movement::GetDirection() const
{
	return Direction4::DIRECTION4_DOWN;
}

int Movement::GetDirection8() const
{
	return Direction8::DIRECTION8_DOWN;
}
