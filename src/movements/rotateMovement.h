#ifndef _ROTATE_MOVEMENT_H_
#define _ROTATE_MOVEMENT_H_

#include"movements\movement.h"

/**
*	\brief Ðý×ª¶¯×÷
*/
class RotateMovement : public Movement
{
public:
	RotateMovement();
	RotateMovement(float angleAmount, float rotateSpeed);

	virtual void Update();
	virtual void SetSuspended(bool suspended);

	void SetRotateSpeed(float speed);
	float GetRotateSpeed()const;
	void SetAngleAmount(float amount);

	virtual bool IsFinished()const;
	virtual void Stop();
	void SetFinished(bool finished);

	// lua
	virtual const string GetLuaObjectName()const;

private:
	float mRotateSpeed;
	float mAngleAmount;
	float mCurAngle;
	int mRotateDirection;
	bool mFinished;

	uint32_t mNextRotateDate;
	uint32_t mRotateDelay;
};

#endif