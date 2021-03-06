#include "freeState.h"
#include "entity\player.h"
#include "player\playerSprite.h"

FreeState::FreeState(Entity & entity):
	MovementState(entity, "NormalMovement")
{
}

void FreeState::Start(EntityState & state)
{
	MovementState::Start(state);
}

void FreeState::Stop(EntityState & state)
{
	MovementState::Stop(state);
}

void FreeState::Update()
{
	MovementState::Update();
}

void FreeState::NotifyCommandPressed(const GameCommand & command)
{
}

void FreeState::NotifyCommandReleased(const GameCommand & command)
{
}

void FreeState::SetPlayerWalkingAnimation()
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.GetPlayerSprites().SetWalkingNormalAnimation();
}

void FreeState::SetPlayerStopAnimation()
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.GetPlayerSprites().SetStopNormalAnimation();;
}

/**
*	\brief 自由状体下可以攻击
*/
bool FreeState::CanAttack() const
{
	return true;
}


