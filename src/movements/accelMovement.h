#pragma once

#include"movements\movement.h"
#include"utils\vec2.h"

/**
*	\brief ����Բ�ܶ���
*
*	�����˶������ڼ��ٶȵĹ�ϵ����Ҫÿ֡���Ǹı��ٶ�
*	�������˶���ͬ�����ǲ��ü�����ټ���ı�1���أ�
*	���Ǽ���ÿ֡�ƶ��������أ���ÿ֡�ƶ������ع���ʱ��
*	��ײ�����кܴ�������Ա����˶�����ʹ���ڲ�����
*	��ײ���ĳ��������������˶�
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
	double mRadialAccel;		// ���ļ��ٶ�
	double mTangentialAccel;	// ������ٶ�
	double mGravityAccel;		// �������ٶ�
	Point2 mOrigin;				// ������ԭ��λ��
	Vec2d  mSpeed;
	double mAngle;

	double mDistanceX;
	double mDistanceY;
};
