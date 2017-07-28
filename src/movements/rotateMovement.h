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
	RotateMovement(double angleAmount, double angularSpeed);

	virtual void Update();
	virtual void SetSuspended(bool suspended);

private:
	double mAngularSpeed;
	
};

#endif