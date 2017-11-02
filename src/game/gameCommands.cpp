#include "game\gameCommands.h"
#include "game\game.h"
#include "game\savegame.h"
#include "game\enumInfo.h"

std::map<GameCommand, std::string> GameCommands::mGameCommands = {
	{ GameCommand::GAME_COMMAND_RIGHT,"right" },
	{ GameCommand::GAME_COMMAND_UP,   "up" },
	{ GameCommand::GAME_COMMAND_LEFT, "left" },
	{ GameCommand::GAME_COMMAND_DOWN, "down" },
	{ GameCommand::GAME_COMMAND_INTERACT, "interact"}
};

namespace {

	/**
	*	4方向的按键的掩码
	*/
	const uint32_t direction4Mask[4] = 
	{
		0x0001,		// right
		0x0002,		// up
		0x0004,		// left
		0x0008		// down
	};

	/**
	*	按键的位码，应以判断当多个方向键按下时的
	*	移动方向，其中返回的数值对应direction8的
	*   枚举值，其中-1为无效方向	
	*/
	const Direction8 direction8Masks[16] = {
		DIRECTION8_NONE,		// no pressed
		DIRECTION8_RIGHT,		// right
		DIRECTION8_UP,			// up
		DIRECTION8_RIGHT_UP,	// right + up
		DIRECTION8_LEFT,		// left
		DIRECTION8_NONE,		// left + right
		DIRECTION8_UP_LEFT,	    // up + left
		DIRECTION8_NONE,		// up + right + left
		DIRECTION8_DOWN,		// down
		DIRECTION8_DOWN_RIGHT,	// down + right
		DIRECTION8_NONE,		// down + up
		DIRECTION8_NONE,		// down + right + up
		DIRECTION8_LEFT_DOWN,	// down + left
		DIRECTION8_NONE,		// down + left + right
		DIRECTION8_NONE,		// down + left + up
		DIRECTION8_NONE,		// down + left + right + up
	};
}

GameCommands::GameCommands(Game& game):
	mGame(game)
{
	// 根据command从savegame获取映射的key
	for (auto kvp : mGameCommands)
	{
		GameCommand gameCommand = kvp.first;
		// 映射键盘按键
		InputEvent::KeyboardKey key = GetSavegameMappingKeyboard(gameCommand);
		if (key != InputEvent::KEY_NONE)
		{
			mKeyboardMapping[key] = gameCommand;
		}
		// 映射鼠标按键
	}
}

/**
*	\brief 响应输入事件，根据映射关系获取到gameCommand
*/
void GameCommands::NotifyInput(const InputEvent & ent)
{
	auto keyEvent = ent.GetKeyEvent();
	if (keyEvent.type == InputEvent::EventType::EVENT_KEYBOARD_KEYDOWN)
	{
		KeyBoardKeyPressed(keyEvent.key);
	}
	else if (keyEvent.type == InputEvent::EventType::EVENT_KEYBOARD_KEYUP)
	{
		KeyBoardKeyReleased(keyEvent.key);
	}
	else if (keyEvent.type == InputEvent::EventType::EVENT_MOUSE_BUTTONDOWN)
	{
	}
	else if (keyEvent.type == InputEvent::EventType::EVENT_MOUSE_BUTTONUP)
	{
	}
	else if (keyEvent.type == InputEvent::EventType::EVENT_MOUSE_MOTION)
	{
	}
}

void GameCommands::GameCommandPressed(const GameCommand & command)
{
	mGameCommandPressed.insert(command);
	mGame.NotifyGameCommandPressed(command);
}

void GameCommands::GameCommandReleased(const GameCommand & command)
{
	mGameCommandPressed.erase(command);
	mGame.NotifyGameCommandReleased(command);
}

Savegame & GameCommands::GetSavegame()
{
	return mGame.GetSavegame();
}

/**
*	\brief 返回某个command是否被按下
*/
bool GameCommands::IsGameCommandPressed(GameCommand command)const
{
	auto it = mGameCommandPressed.find(command);
	return it != mGameCommandPressed.end();
}

/**
*	\brief 根据当前gameCommandPressed获取当前8方向方向
*/
Direction8 GameCommands::GetWantedDirection8() const
{
	uint32_t directionMask = 0;
	if (IsGameCommandPressed(GameCommand::GAME_COMMAND_RIGHT))
	{
		directionMask |= direction4Mask[0];
	}
	if (IsGameCommandPressed(GameCommand::GAME_COMMAND_UP))
	{
		directionMask |= direction4Mask[1];
	}
	if (IsGameCommandPressed(GameCommand::GAME_COMMAND_LEFT))
	{
		directionMask |= direction4Mask[2];
	}
	if (IsGameCommandPressed(GameCommand::GAME_COMMAND_DOWN))
	{
		directionMask |= direction4Mask[3];
	}
	return direction8Masks[directionMask];
}

/**
*	\brief 响应键盘按下事件
*/
void GameCommands::KeyBoardKeyPressed(const InputEvent::KeyboardKey & key)
{
	// 找到key对应的command，并触发gameCommand事件
	auto it = mKeyboardMapping.find(key);
	if (it != mKeyboardMapping.end())
	{
		GameCommand command = it->second;
		GameCommandPressed(command);
	}
}

/**
*	\brief 响应键盘放开事件
*/
void GameCommands::KeyBoardKeyReleased(const InputEvent::KeyboardKey & key)
{
	// 找到key对应的command，并触发gameCommand事件
	auto it = mKeyboardMapping.find(key);
	if (it != mKeyboardMapping.end())
	{
		GameCommand command = it->second;
		GameCommandReleased(command);
	}
}

InputEvent::KeyboardKey GameCommands::GetSavegameMappingKeyboard(GameCommand command)
{
	const static std::map<GameCommand, std::string> savegameKeyboardValue = {
		{ GameCommand::GAME_COMMAND_UP,   Savegame::GAMECOMMAND_KEYBOARD_UP },
		{ GameCommand::GAME_COMMAND_RIGHT,Savegame::GAMECOMMAND_KEYBOARD_RIGHT },
		{ GameCommand::GAME_COMMAND_DOWN, Savegame::GAMECOMMAND_KEYBOARD_DOWN },
		{ GameCommand::GAME_COMMAND_LEFT, Savegame::GAMECOMMAND_KEYBOARD_LEFT },
		{ GameCommand::GAME_COMMAND_INTERACT, Savegame::GAMECOMMAND_KEYBOARD_INTERACT}
	};

	std::string keyStr = "";
	auto it = savegameKeyboardValue.find(command);
	if (it != savegameKeyboardValue.end())
	{
		const std::string savegameValue = it->second;
		keyStr = GetSavegame().GetString(savegameValue);
	}
	return StringToEnum<InputEvent::KeyboardKey>(keyStr, InputEvent::KEY_NONE);
}
