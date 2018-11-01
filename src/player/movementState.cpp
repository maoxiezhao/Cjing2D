#include "movementState.h"
#include "entity\player.h"
#include "player\playerSprite.h"
#include "lua\luaContext.h"

MovementState::MovementState(Entity & entity, const std::string& name):
	PlayerState(entity, name),
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
	mDirection = static_cast<Direction4>(mPlayerMovement->GetDirection());
}

/**
*	\brief 返回4方向，和8方向处理不同，这里以成员的方式记录
*	当前移动方向，用于在其他StateStart时获取方向（因此此时PlayerMovment已经Clear）
*/
Direction4 MovementState::GetWantedDirection4() const
{
	return mDirection;
}

Direction8 MovementState::GetWantedDirection8() const
{
	Debug::CheckAssertion(mPlayerMovement != nullptr,
		"Used movement without initialized.");

	return static_cast<Direction8>( mPlayerMovement->GetDirection8());
}

void MovementState::NotifyMovementChanged()
{
	Direction8 wantedDirection = static_cast<Direction8>(
		mPlayerMovement->GetDirection8());

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
	auto& luaContext = *player.GetLuaContext();
	bool canShift = luaContext.CallFunctionWithUserdata(player, "OnBeforeShiftMove",
		[&](lua_State*l)->int {
		luaContext.PushUserdata(l, player);
		return 1;
	});

	if (!canShift)
		return;

	mPlayerMovement->StartShift(player.GetShiftSpeed());
	luaContext.CallFunctionWithUserdata(player, "OnAfterShiftMove",
		[&](lua_State*l)->int {
		luaContext.PushUserdata(l, player);
		return 1;
	});
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
