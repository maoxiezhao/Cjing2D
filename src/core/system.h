#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include"common\common.h"

class System
{
public:
	static uint32_t Now();
	static uint32_t RealTime();
	static void Sleeps(uint32_t duration);

	static void Initialize();
	static void Update();
	static void Quit();

	static const uint32_t timeStep = 17;
	static const uint32_t clickDeltaTime = 200;
	static const uint32_t doubleClickDeltaTime = 500;
private:
	static uint32_t mInitDate;
	static uint32_t mTicks;
};

#endif