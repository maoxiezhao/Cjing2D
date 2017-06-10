#include"game.h"
#include"core\inputEvent.h"
#include"game\app.h"
#include"game\savegame.h"

Game::Game(App* app):	// test,no savegame
	mStarted(false)
{
	mPlayer = std::make_shared<Player>();
}

Game::Game(App* app, const std::shared_ptr<Savegame> savegame):
	mStarted(false)
{
	// º”‘ÿplayer
	mPlayer = std::make_shared<Player>();

	// º”‘ÿmap
}

void Game::Start()
{
	if (mStarted)
	{
		return;
	}

	mStarted = true;
}

void Game::Stop()
{
	if (!mStarted)
	{
		return;
	}

	mStarted = false;
}

void Game::Restart()
{
}

void Game::Update()
{
}

void Game::Draw()
{
}

void Game::NotifyInput(const InputEvent & ent)
{
}