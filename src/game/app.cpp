#include"app.h"
#include"core\debug.h"
#include"core\logger.h"
#include"core\system.h"
#include"core\fileData.h"
#include"core\video.h"
#include"core\renderer.h"
#include"core\fontAtlas.h"

#include<Windows.h>

//test
#include"movements\targetMovement.h"

App::App() :
	mLuaContext(nullptr),
	mExiting(false),
	mCurrGame(nullptr),
	mNextGame(nullptr),
	mAsyncLoaded(false)
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


	mLoadingAnimate = std::make_shared<AnimationSprite>("menus/loading");
	mLoadingAnimate->SetSize({ 120,120 });
	mLoadingAnimate->SetPos({ (DEFAULT_WINDOW_WIDTH - mLoadingAnimate->GetSize().width )/ 2, (DEFAULT_WINDOW_HEIGHT - mLoadingAnimate->GetSize().height) / 2});
	mLoadingAnimate->StartAnimation();

	mAsyncLoader = std::make_shared<AsyncLoader>();
	mAsyncLoader->AddTask(std::bind(&App::AsyncLoading, this));
	mAsyncLoader->SetFinishCallBack(std::bind(&App::LoadingFinishCallBack, this));
	mAsyncLoader->Run();

}

App::~App()
{
	// 首先停止异步加载器，未来如果加载器一直到lua场景中
	// 则可不进行该操作
	mAsyncLoader->Stop();

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
			Step();
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

/**
*	\brief 为了支持异步加载
*
*	目前因为游戏场景主要由lua逻辑来构建，所以异步加载的逻辑目前只能
*	耦合在app中
*/
void App::Step()
{
	if (!mAsyncLoaded)
	{
		mLoadingAnimate->Update();
		mAsyncLoader->Update();
	}
	Update();
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

	// async loader
	if (!mAsyncLoaded)
	{
		mLoadingAnimate->Draw();
	}
	else
	{	// game draw
		if (mCurrGame != nullptr)
		{
			mCurrGame->Draw();
		}
		mLuaContext->OnMainDraw();

		std::unique_ptr<InputEvent> ent = InputEvent::GetSingleEvent(InputEvent::EVENT_DRAW);
		mGUI->HandleEvent(*ent);

		mWidget1->DrawBackground();
		mWidget2->DrawBackground();
		mWidget3->DrawBackground();

		mText->Draw();
	}

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
	if (mGUI != nullptr)
	{
		mGUI->HandleEvent(ent);
	}
}

/**
*	\brief 动态加载执行的函数
*/
void App::AsyncLoading()
{
	Logger::Info("Async load default font");
	font::FontAtlas::GetInstance().LoadDefaultFont();
	Logger::Info("Async load default font succeed");

	// test 
	mText = std::make_shared<TextDrawable>();
	mText->SetFont(font::FontAtlas::GetInstance().GetDefaultFont());
	mText->SetText(u8"苟且于生活\n带风流浪\n心没有归属\n流浪四方");
	mText->SetTextHorizontalAlign(font::TEXT_ALIGN_CENTER);
	mText->SetPos(Point2(300, 100));
	mText->SetLineHeight(30);
	mText->SetLetterSpacing(2);
	// test
	// init gui system
	mGUI = std::unique_ptr<gui::GUIManager>(new gui::GUIManager());
	mGrid = std::make_shared<gui::Grid>(3, 3);
	mGrid->Connect();

	mWidget1 = std::make_shared<gui::Widget>();
	mWidget1->Connect();
	mWidget1->SetWantKeyboard(true);
	mWidget1->SetMouseBehavior(gui::Dispatcher::all);
	mWidget1->Place(Point2(0, 0), Size(100, 100));

	mWidget2 = std::make_shared<gui::Widget>();
	mWidget2->Connect();
	mWidget2->Place(Point2(0, 0), Size(100, 100));

	mWidget3 = std::make_shared<gui::Widget>();
	mWidget3->Connect();
	mWidget3->Place(Point2(0, 0), Size(100, 100));

	//// grid
	mGrid->SetChildren(mWidget1, 1, 0, gui::ALIGN_HORIZONTAL_BOTTOM | gui::ALIGN_VERTICAL_CENTER, 0);
	mGrid->SetChildren(mWidget2, 1, 1, gui::ALIGN_HORIZONTAL_CENTER | gui::ALIGN_VERTICAL_CENTER, 0);
	mGrid->SetChildren(mWidget3, 1, 2, gui::ALIGN_HORIZONTAL_TOP | gui::ALIGN_VERTICAL_CENTER, 0);
	mGrid->Place(Point2(0, 0), Size(640, 480));
}

/**
*	\brief 动态加载成功后的回调函数
*/
void App::LoadingFinishCallBack()
{
	mAsyncLoaded = true;
	Logger::Info("Async load Finished.");
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


