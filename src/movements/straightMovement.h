#ifndef _STRAIGHT_MOEVMENT_H_
#define _STRAIGHT_MOVEMENT_H_

#include"common\common.h"
#include"movements\movement.h"
#include"utils\vec2.h"

/**
*	\brief 直线移动
*/
class StraightMovement : public Movement
{
public:
	StraightMovement();
	StraightMovement(bool isIngroedObstacles);

	virtual void Update();
	virtual void SetSuspended(bool suspended);

	// status
	void SetAngle(double angle);
	void SetSpeed(double speed);
	void SetSpeedVec2(const Vec2f& speed);
	void SetSpeedX(double speedX );
	void SetSpeedY(double speedY );
	void SetMaxDistance(double distance);
	double GetMaxDistance()const;
	double GetAngle()const;
	double GetSpeed()const;
	virtual bool IsStarted()const;

	// move
	virtual bool IsFinished()const;
	virtual void Stop();
	void SetFinished(bool finished);
	void SetNextMoveDateX(uint32_t date);
	void SetNextMoveDateY(uint32_t date);

	// lua
	virtual const string GetLuaObjectName()const;

protected:
	void UpdateXY();
	void UpdateSmoothX();
	void UpdateSmoothY();

private:
	double mSpeedX;
	double mSpeedY;
	double mAngle;

	int mDirectionX;	// x方向的移动方向，0:不移动，1正向移动，-1负向移动
	int mDirectionY;	// y方向的移动方向，0:不移动，1正向移动，-1负向移动
	bool mFinished;

	uint32_t mNextMoveDateX;
	uint32_t mNextMoveDateY;

	uint32_t mDelayX;
	uint32_t mDelayY;

	Point2 mStartedPoint;
	double mMaxDistance;
};


#endif