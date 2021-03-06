#include "entityState.h"
#include "game\game.h"

EntityState::EntityState(Entity & entity, const std::string& name):
	mEntity(entity),
	mName(name),
	mIsSuspended(false)
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

void EntityState::SetSuspended(bool suspended)
{
	if (mIsSuspended != suspended)
	{
		mIsSuspended = suspended;
	}
}

bool EntityState::IsSuspended() const
{
	return mIsSuspended;
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
	case GameCommand::GAME_COMMAND_SHIFT:
		NotifyCommandShiftPressed();
		break;
	case GameCommand::GAME_COMMAND_ATTACK:
		NotifyCommandAttackPressed();
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

void EntityState::NotifyMovementFinished()
{
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

void EntityState::NotifyCommandShiftPressed()
{
}

void EntityState::NotifyCommandAttackPressed()
{
}

void EntityState::NotifyStopGrabbing()
{
}

std::string EntityState::GetName() const
{
	return mName;
}

Direction8 EntityState::GetWantedDirection8() const
{
	return Direction8::DIRECTION8_NONE;
}

Direction4 EntityState::GetWantedDirection4() const
{
	return Direction4::DIRECTION4_NONE;
}

bool EntityState::CanAttack() const
{
	return false;
}

float EntityState::GetFacingDegree() const
{
	return 0.0f;
}

Game & EntityState::GetGame()
{
	return mEntity.GetGame();
}

const Game & EntityState::GetGame() const
{
	return mEntity.GetGame();
}

Entity & EntityState::GetEntity()
{
	return mEntity;
}

const Entity & EntityState::GetEntity() const
{
	return mEntity;
}
