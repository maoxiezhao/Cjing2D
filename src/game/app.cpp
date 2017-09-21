#include"app.h"
#include"core\debug.h"
#include"core\logger.h"
#include"core\system.h"
#include"core\fileData.h"
#include"core\configData.h"
#include"core\video.h"
#include"core\renderer.h"
#include"core\fontAtlas.h"

#include<Windows.h>

//test
#include"movements\targetMovement.h"
#include"gui\widget\selections.h"
#include"gui\widget\selections_private.h"
#include"gui\widget\toggleButton.h"

App::App() :
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

	Logger::Info("Open Config.");
	ConfigData::LoadConfig("config.dat");
	FileData::SetDataWriteDir(ConfigData::GetConfigProperties().GetWriteDir());

	// initialize system
	Logger::Info("Initialize system modules");
	System::Initialize();

	// initialize lua
	Logger::Info("Initialize Lua context");
	mLuaContext = std::unique_ptr<LuaContext>(new LuaContext(this));
	mLuaContext->Initialize();

	// initialize gui manager
	Logger::Info("Initialize GUI system");
	mGUI = std::unique_ptr<gui::GUIManager>(new gui::GUIManager());
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
*	\brief ��ѭ������

*	�ڸú����и��ݹ̶���Ƶ��ѭ��ˢ��
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

		// ����֡
		if (lastFrameDuration > 0)
		{
			Video::SetFPS(uint32_t(1000 / lastFrameDuration));
			//std::cout << Video::GetFPS() << std::endl;
		}
		// �п�����ΪһЩ�����������ش��ļ�)������һ֡ʱ��
		// �ر𳤣������׷�أ���ʱ���¼��TimeDropped
		if (lag >= 300)
		{
			timeDropped += lag - System::timeStep;
			lag = System::timeStep;
			lastFrameDate = System::RealTime() - timeDropped;
		}

		// 1.check input
		glfwPollEvents();
		CheckInput();

		// 2.update	�������Ļ�������Ҫѭ����ζ�����һЩ���ƹ���
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
*	\brief �������ڿ���update����
*/
void App::Step()
{
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
		// �л���ǰgame
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
*	\brief ��ȡinputEvent
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
*	\brief ִ�л��Ʋ���
*/
void App::Render()
{
	Video::CleanCanvas();

	if (mCurrGame != nullptr)
	{
		mCurrGame->Draw();
	}

	std::unique_ptr<InputEvent> ent = InputEvent::GetSingleEvent(InputEvent::EVENT_DRAW);
	mGUI->HandleEvent(*ent);

	mLuaContext->OnMainDraw();

	Video::Rendercanvas();
}

/**
*	\brief ��ӦinputEvent����event�ַ����ⲿ
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
*	\brief ���õ�ǰ��Ϸ
*   \param game ��gameΪnullptr���˳���Ϸ
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


