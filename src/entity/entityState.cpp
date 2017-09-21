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
}

void EntityState::NotifyCommandReleased(const GameCommand & command)
{
}
