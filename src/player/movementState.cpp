#include "movementState.h"
#include "entity\player.h"
#include "player\playerSprite.h"
#include "lua\luaContext.h"

MovementState::MovementState(Entity & entity):
	PlayerState(entity),
	mPlayerMovement(nullptr)
{
}

void MovementState::Start(EntityState & state)
{
	auto& player = GetPlayer();
	mPlayerMovement = std::make_shared<PlayerMovement>(player.GetWalkingSpeed());
	player.StartMovement(mPlayerMovement);
}

void MovementState::Stop(EntityState & state)
{
	auto& player = GetPlayer();
	player.ClearMovements();
	mPlayerMovement = nullptr;
}

void MovementState::Update()
{
	PlayerState::Update();
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

	// 设置快速位移动画
	if (mPlayerMovement->IsShifting())
	{
		SetPlayerShiftingAnimation();
	}
}

void MovementState::NotifyCommandShiftPressed()
{
	auto& player = GetPlayer();
	mPlayerMovement->StartShift(player.GetShiftSpeed());
	player.NotifyShiftMove();
}

void MovementState::SetPlayerWalkingAnimation()
{
	auto& player = GetPlayer();
	player.GetPlayerSprites().SetWalkingNormalAnimation();
}

void MovementState::SetPlayerStopAnimation()
{
	auto& player = GetPlayer();
	player.GetPlayerSprites().SetStopNormalAnimation();;
}

void MovementState::SetPlayerShiftingAnimation()
{
	auto& player = GetPlayer();
	player.GetPlayerSprites().SetShiftNormalAnimation();
}
