#ifndef _MOVEMENT_H_
#define _MOVEMENT_H_

#include"common\common.h"
#include"utils\point.h"
#include"utils\rectangle.h"
#include"lua\luaRef.h"
#include"lua\luaObject.h"

class Drawable;
class Entity;

/**
*	\brief 移动插件的抽象基类,可以作为插件移动drawable和entity
*/
class Movement : public LuaObject
{
public:
	virtual ~Movement();

	virtual void Update();
	bool IsSuspended()const;
	virtual void SetSuspended(bool suspended);
	
	// status
	Entity* GetEntity();
	Drawable* GetDrawable();

	void RefreshPos();
	int GetPosX()const;
	int GetPosY()const;
	Point2 GetPos()const;
	void SetX(const int x);
	void SetY(const int y);
	void SetPos(const Point2& pos);
	void TranslateX(int x);
	void TranslateY(int y);
	void TranslatePos(const Point2& dxy);
	uint32_t GetWhenSuspeneded()const;
	virtual void SetDrawable(Drawable* drawable);
	virtual void SetEntity(Entity* entity);

	// display
	virtual int GetDirection()const;

	// move
	virtual bool IsFinished()const;
	virtual void Start();
	virtual void Stop();
	virtual bool IsStop()const;
	virtual bool IsStarted()const;

	// lua
	virtual void NotifyPositonChanged();
	virtual void NotifyMovementChanged();
	virtual void NotifyMovementFinished();
	virtual void NotifyObstacleReached();
	virtual const string GetLuaObjectName()const;

	void SetFinishedCallBack(const LuaRef& callback);

	// test collision
	bool TestCollisionWithObstacles(const Point2& dxy)const;
	bool IsIngoredObstacles()const;
	void SetIngoredObstacles(bool ingoredObstacles);

protected:
	Movement();
	Movement(bool ingoreObstacles);

private:
	Point2 mPos;
	bool mFinished;
	bool mSuspended;
	uint32_t mWhenSuspendedTime;
	bool mIngoreObstacle;


	Drawable* mDrawable;
	Entity* mEntity;

	LuaRef mFinishedCallBack;
};

using MovementPtr = std::shared_ptr<Movement>;

#endif