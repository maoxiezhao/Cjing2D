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
		NotifyCommandDirectionPressed(Direction4::DIRECTION4_UP);
		break;
	case GameCommand::GAME_COMMAND_RIGHT:
		NotifyCommandDirectionPressed(Direction4::DIRECTION4_RIGHT);
		break;
	case GameCommand::GAME_COMMAND_DOWN:
		NotifyCommandDirectionPressed(Direction4::DIRECTION4_DOWN);
		break;
	case GameCommand::GAME_COMMAND_LEFT:
		NotifyCommandDirectionPressed(Direction4::DIRECTION4_LEFT);
		break;
	case GameCommand::GAME_COMMAND_INTERACT:
		NotifyCommandInteractPressed();
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
		NotifyCommandDirectionReleased(Direction4::DIRECTION4_UP);
		break;
	case GameCommand::GAME_COMMAND_RIGHT:
		NotifyCommandDirectionReleased(Direction4::DIRECTION4_RIGHT);
		break;
	case GameCommand::GAME_COMMAND_DOWN:
		NotifyCommandDirectionReleased(Direction4::DIRECTION4_DOWN);
		break;
	case GameCommand::GAME_COMMAND_LEFT:
		NotifyCommandDirectionReleased(Direction4::DIRECTION4_LEFT);
		break;
	case GameCommand::GAME_COMMAND_INTERACT:
		
		break;
	default:
		break;
	}
}

void EntityState::NotifyMovementChanged()
{
	// do nothing
}

void EntityState::NotifyCommandDirectionPressed(Direction4 direction)
{
	// do nothing
}

void EntityState::NotifyCommandDirectionReleased(Direction4 direction)
{
	// do nothing
}

void EntityState::NotifyCommandInteractPressed()
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
