#pragma once

#include"common\common.h"
#include"core\inputEvent.h"

class Game;
class Savegame;

/**
*	\brief 4方向枚举
*/
enum Direction4
{
	DIRECTION4_NONE = -1,
	DIRECTION4_RIGHT = 0,
	DIRECTION4_UP,
	DIRECTION4_LEFT,
	DIRECTION4_DOWN,
};

/**
*	\brief 8方向枚举
*/
enum Direction8
{
	DIRECTION8_NONE  = -1,
	DIRECTION8_RIGHT = 0,
	DIRECTION8_RIGHT_UP,
	DIRECTION8_UP,
	DIRECTION8_UP_LEFT,
	DIRECTION8_LEFT,
	DIRECTION8_LEFT_DOWN,
	DIRECTION8_DOWN,
	DIRECTION8_DOWN_RIGHT,
};

/**
*	\brief 游戏动作命令
*/
enum class GameCommand
{
	GAME_COMMAND_RIGHT,
	GAME_COMMAND_UP,
	GAME_COMMAND_LEFT,
	GAME_COMMAND_DOWN,
	GAME_COMMAND_INTERACT,
	GAME_COMMAND_SHIFT,
	GAME_COMMAND_ATTACK,
};

/**
*	\brief 游戏动作命令管理
*
*	游戏命令管理器管理者当前设置了的游戏命令，并将游戏命令和
*	对应的输入事件实现映射。GameCommands 仅在game中存在和响应
*
*	同时，这里根据当前方向的输入得到当前的移动方向，其他地方
*	可以获取到改方向
*/
class GameCommands
{
public:
	GameCommands(Game& game);

	void NotifyInput(const InputEvent & ent);
	void GameCommandPressed(const GameCommand& command);
	void GameCommandReleased(const GameCommand& command);

	Savegame& GetSavegame();

	// status
	bool IsGameCommandPressed(GameCommand command)const;
	Direction8 GetWantedDirection8()const;

private:
	void KeyBoardKeyPressed(const InputEvent::KeyboardKey& key);
	void KeyBoardKeyReleased(const InputEvent::KeyboardKey& key);
	void MouseKeyPressed(const InputEvent::MouseButton& button);
	void MouseKeyReleased(const InputEvent::MouseButton& button);

	InputEvent::KeyboardKey GetSavegameMappingKeyboard(GameCommand command);

private:
	// 定义游戏中的命令，和对应的字符名称，用于在savegame中存取
	static std::map<GameCommand, std::string> mGameCommands;

	// 各个输入键值对应的gameCommand
	std::map<InputEvent::KeyboardKey, GameCommand> mKeyboardMapping;
	std::map<InputEvent::MouseButton, GameCommand> mMouseButtonMapping;

	Game& mGame;
	std::set<GameCommand> mGameCommandPressed;
};