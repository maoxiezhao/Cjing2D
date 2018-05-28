#include"core\system.h"
#include"core\inputEvent.h"
#include"core\video.h"
#include"core\sound.h"
#include"core\random.h"
#include"utils\reflect.h"
#include<Windows.h>

uint32_t System::mInitDate = 0;
uint32_t System::mTicks = 0;

uint32_t System::Now()
{
	return mTicks;
}

uint32_t System::RealTime()
{
	//return (uint32_t)GetTickCount();
	return (uint32_t)(glfwGetTime() * 1000);
}

void System::Sleeps(uint32_t duration)
{
	Sleep(duration);
}


void System::Initialize()
{
	mInitDate = RealTime();
	mTicks = 0;
	glfwSetTime(0.0);

	auto& metaManager = util::refel::MetaManager::GetInstance();
	metaManager.Initialize();
	metaManager.RegisterClass<int>(GL_INT);
	metaManager.RegisterClass<float>(GL_FLOAT);
	metaManager.RegisterClass<ubyte>(GL_UNSIGNED_BYTE);

	// ���γ�ʼ��input,audio,video
	InputEvent::Initialize();
	Sound::Initialize();
	Video::Initialize();
	Random::Initialize();
}

void System::Update()
{
	mTicks += timeStep;

	Sound::Update();
}

void System::Quit()
{
	Video::Quit();
	Sound::Quit();
	InputEvent::Quit();
}
