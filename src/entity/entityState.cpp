#include "entityState.h"

EntityState::EntityState(Entity & entity):
	mEntity(entity)
{
}

void EntityState::Start(EntityState & state)
{
}

void EntityState::Stop(EntityState & state)
{
}

void EntityState::Update()
{
}

void EntityState::NotifyCommandPressed(const GameCommand & command)
{
	switch (command)
	{
	case GameCommand::GAME_COMMAND_UP:

		break;
	case GameCommand::GAME_COMMAND_RIGHT:

		break;
	case GameCommand::GAME_COMMAND_DOWN:

		break;
	case GameCommand::GAME_COMMAND_LEFT:

		break;
	default:
		break;
	}
}

void EntityState::NotifyCommandReleased(const GameCommand & command)
{
	switch (command)
	{
	case GameCommand::GAME_COMMAND_UP:

		break;
	case GameCommand::GAME_COMMAND_RIGHT:

		break;
	case GameCommand::GAME_COMMAND_DOWN:

		break;
	case GameCommand::GAME_COMMAND_LEFT:

		break;
	default:
		break;
	}
}

void EntityState::NotifyMovementChanged()
{
	// do nothing
}

Direction8 EntityState::GetWantedDirection8() const
{
	return Direction8::DIRECTION8_NONE;
}

Entity & EntityState::GetEntity()
{
	return mEntity;
}
