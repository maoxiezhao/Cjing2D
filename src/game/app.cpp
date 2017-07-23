#include"app.h"
#include"core\debug.h"
#include"core\logger.h"
#include"core\system.h"
#include"core\fileData.h"
#include"core\video.h"
#include"core\renderer.h"
#include<Windows.h>

//test
#include"movements\targetMovement.h"

App::App():
	mLuaContext(nullptr),
	mExiting(false),
	mCurrGame(nullptr),
	mNextGame(nullptr)
{
	Logger::Info("Cjing start initializing.");
	
	Logger::Info("Open data file");
	string dataPath = ".";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direcion:" + dataPath);
	
	// initialize system
	Logger::Info("Initialize system modules");
	System::Initialize();

	// initialize render
	Logger::Info("Initialize render system");

	// initialize lua
	mLuaContext = std::unique_ptr<LuaContext>(new LuaContext(this));
	mLuaContext->Initialize();

	// test
	//testAnimation = std::make_shared<AnimationSprite>("sprites/explosion.dat");
	//testAnimation->SetPos(Point2(0, 0));
	//
	//testMovement = std::make_shared<TargetMovement>(nullptr, Point2(300, 300), 25, false);
	//testAnimation->StartMovement(testMovement);
}

App::~App()
{
	if (mCurrGame != nullptr)
	{
		mCurrGame->Stop();
		mCurrGame.reset();
	}
	if (mLuaContext != nullptr)
	{
		mLuaContext->Exit();
	}

	FileData::CloseData();
	System::Quit();
}

/**
*	\brief 主循环函数

*	在该函数中根据固定的频率循环刷新
*/
void App::Run()
{
	Logger::Info("Simulation started");

	uint32_t lastFrameDate = System::RealTime();
	uint32_t lag = 0;
	uint32_t timeDropped = 0;

	while (!IsExiting())
	{
		uint32_t now = System::RealTime() - timeDropped;
		uint32_t lastFrameDuration = now - lastFrameDate;
		lastFrameDate = now;
		lag += lastFrameDuration;

		// 有可能因为一些情况（比如加载大文件)导致这一帧时间
		// 特别长，则放弃追回，将时间记录在TimeDropped
		if (lag >= 200)
		{
			timeDropped += lag - System::timeStep;
			lag = System::timeStep;
			lastFrameDate = System::RealTime() - timeDropped;
		}

		// 1.check input
		glfwPollEvents();
		CheckInput();

		// 2.update	对于慢的机器，需要循环多次而跳过一些绘制过程
		int updates = 0;
		while (lag >= System::timeStep && updates < 10 && !IsExiting())
		{
			Update();
			lag -= System::timeStep;
			++updates;
		}

		// 3.render
		if(updates > 0)
			Render();

		// 4.sleep
		lastFrameDuration = (System::RealTime() - timeDropped) - lastFrameDate;
		if (lastFrameDuration < System::timeStep)
			System::Sleeps((System::timeStep - lastFrameDuration));
	}

	Logger::Info("Simulation finished");
}

void App::Update()
{
	// game update
	if (mCurrGame != nullptr)
	{
		mCurrGame->Update();
	}
	mLuaContext->Update();
	System::Update();

	if (mNextGame != mCurrGame.get())
	{
		// 切换当前game
		mCurrGame = std::unique_ptr<Game>(mNextGame);
		if (mCurrGame != nullptr)
		{
			mCurrGame->Start();
		}
		else
		{
			mLuaContext->Exit();
		}
	}
}

void App::SetExiting(bool isexit)
{
	mExiting = isexit;
}

bool App::IsExiting()
{
	return mExiting || Video::IsExiting();
}

/**
*	\brief 获取inputEvent
*/
void App::CheckInput()
{
	std::unique_ptr<InputEvent> ent = InputEvent::GetEvent();
	while (ent != nullptr)
	{
		NotifyInput(*ent);
		ent = InputEvent::GetEvent();
	}
}

/**
*	\brief 执行绘制操作
*/
void App::Render()
{
	Video::CleanCanvas();

	if (mCurrGame != nullptr)
	{
		mCurrGame->Draw();
	}
	mLuaContext->OnMainDraw();

	Video::Rendercanvas();
}

/**
*	\brief 响应inputEvent，将event分发给外部
*/
void App::NotifyInput(const InputEvent & ent)
{
	if (ent.IsWindowClosing())
		SetExiting(true);
	bool handle = mLuaContext->NotifyInput(ent);
	if ( mCurrGame != nullptr)
	{
		mCurrGame->NotifyInput(ent);
	}
}

/**
*	\brief 设置当前游戏
*   \param game 当game为nullptr则退出游戏
*/
void App::SetGame(Game* game)
{
	mNextGame = game;
}

Game* App::GetGame()
{
	return mCurrGame.get();
}

LuaContext & App::GetLuaContext()
{
	return *mLuaContext;
}


