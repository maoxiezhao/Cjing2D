#pragma once

#include"movements\movement.h"
#include"utils\vec2.h"

/**
*	\brief 变速圆周动作
*
*	变速运动，存在加速度的关系，需要每帧考虑改变速度
*	与匀速运动不同，不是采用计算多少间隔改变1像素，
*	而是计算每帧移动多少像素，当每帧移动的像素过大时，
*	碰撞检测会有很大的误差，所以变速运动尽量使用在不参与
*	碰撞检测的场景，例如粒子运动
*/
class AccelMovement : public Movement
{
public:
	AccelMovement();
	AccelMovement(double radialAccel, double tangentialAccel, double gravityAccel = 0.0, bool ingoreObstacles = false);

	virtual void Update();
	virtual bool IsStarted()const;

	void SetRadialAccel(double radialAccel);
	void SetTangentialAccel(double tangentialAccel);
	void SetGravityAccel(double gravityAccel);

	void SetAngle(double angle);
	void SetSpeed(double speed);
	double GetSpeedVar()const;
	Vec2d GetSpeed()const;
	void SetOrigin(const Point2& pos);

private:
	void UpdateSpeed();
	void UpdateXY();
	//void UpdateX();
	//void UpdateY();

private:
	double mRadialAccel;		// 向心加速度
	double mTangentialAccel;	// 切向加速度
	double mGravityAccel;		// 重力加速度
	Point2 mOrigin;				// 出发的原点位置
	Vec2d  mSpeed;
	double mAngle;

	double mDistanceX;
	double mDistanceY;
};
