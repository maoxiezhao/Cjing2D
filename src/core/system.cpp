#include"system.h"
#include<Windows.h>

uint32_t System::mInitDate = 0;
uint32_t System::mTicks = 0;

uint32_t System::Now()
{
	return mTicks;
}

uint32_t System::RealTime()
{
	return (uint32_t)timeGetTime();
}

void System::Sleeps(uint32_t duration)
{
	Sleep(duration);
}


void System::Initialize()
{
	mInitDate = RealTime();
	mTicks = 0;
}

void System::Update()
{
	mTicks += timeStep;
}

void System::Quit()
{
}
