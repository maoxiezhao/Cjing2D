#include"core\system.h"
#include"core\inputEvent.h"
#include"core\video.h"
#include"core\sound.h"
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
	Sound::Initialize();
	Video::Initialize();
}

void System::Update()
{
	mTicks += timeStep;

	Sound::Update();
}

void System::Quit()
{
	Video::Quit();
	Sound::Quid();
	InputEvent::Quit();
}
