#include "StoppintState.h"
#include "player/playerSprite.h"

StoppingState::StoppingState(Entity & entity):
	PlayerState(entity, "Stopping")
{
}

void StoppingState::Start(EntityState & state)
{
	PlayerState::Start(state);
	GetPlayerSprites().SetStopNormalAnimation();
}
