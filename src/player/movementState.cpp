#include "movementState.h"
#include "entity\player.h"
#include "player\playerSprite.h"

MovementState::MovementState(Entity & entity):
	EntityState(entity),
	mPlayerMovement(nullptr)
{
}

void MovementState::Start(EntityState & state)
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	mPlayerMovement = std::make_shared<PlayerMovement>(player.GetWalkingSpeed());
	player.StartMovement(mPlayerMovement);
}

void MovementState::Stop(EntityState & state)
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.ClearMovements();
	mPlayerMovement = nullptr;
}

void MovementState::Update()
{
}

Direction8 MovementState::GetWantedDirection8() const
{
	Debug::CheckAssertion(mPlayerMovement != nullptr,
		"Used movement without initialized.");

	return mPlayerMovement->GetDirection8();
}

void MovementState::NotifyMovementChanged()
{
	Direction8 wantedDirection = mPlayerMovement->GetDirection8();
	if (wantedDirection == Direction8::DIRECTION8_NONE)
	{
		SetPlayerStopAnimation();
	}
	else
	{
		SetPlayerWalkingAnimation();
	}
}

void MovementState::SetPlayerWalkingAnimation()
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.GetPlayerSprites().SetWalkingNormalAnimation();
}

void MovementState::SetPlayerStopAnimation()
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.GetPlayerSprites().SetStopNormalAnimation();;
}
