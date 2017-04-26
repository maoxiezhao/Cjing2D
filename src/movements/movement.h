#ifndef _MOVEMENT_H_
#define _MOVEMENT_H_

#include"common\common.h"

class Movement
{
public:
	Movement();
	virtual ~Movement();


};

using MovementPtr = std::shared_ptr<Movement>;

#endif