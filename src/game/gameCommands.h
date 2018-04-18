#pragma once

#include"common\common.h"
#include"core\inputEvent.h"

class Game;
class Savegame;

/**
*	\brief 4����ö��
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
*	\brief 8����ö��
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
*	\brief ��Ϸ��������
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
*	\brief ��Ϸ�����������
*
*	��Ϸ��������������ߵ�ǰ�����˵���Ϸ���������Ϸ�����
*	��Ӧ�������¼�ʵ��ӳ�䡣GameCommands ����game�д��ں���Ӧ
*
*	ͬʱ��������ݵ�ǰ���������õ���ǰ���ƶ����������ط�
*	���Ի�ȡ���ķ���
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
	// ������Ϸ�е�����Ͷ�Ӧ���ַ����ƣ�������savegame�д�ȡ
	static std::map<GameCommand, std::string> mGameCommands;

	// ���������ֵ��Ӧ��gameCommand
	std::map<InputEvent::KeyboardKey, GameCommand> mKeyboardMapping;
	std::map<InputEvent::MouseButton, GameCommand> mMouseButtonMapping;

	Game& mGame;
	std::set<GameCommand> mGameCommandPressed;
};