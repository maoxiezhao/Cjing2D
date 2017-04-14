#include"system.h"
#include"inputEvent.h"
#include"video.h"
#include<Windows.h>

uint32_t System::mInitDate = 0;
uint32_t System::mTicks = 0;

uint32_t System::Now()
{
	return mTicks;
}

uint32_t System::RealTime()
{
	return (uint32_t)GetTickCount();
}

void System::Sleeps(uint32_t duration)
{
	Sleep(duration);
}


void System::Initialize()
{
	mInitDate = RealTime();
	mTicks = 0;

	// “¿¥Œ≥ı ºªØinput,audio,video
	InputEvent::Initialize();
	Video::Initialize();
}

void System::Update()
{
	mTicks += timeStep;
}

void System::Quit()
{
	Video::Quit();
	InputEvent::Quit();
}
