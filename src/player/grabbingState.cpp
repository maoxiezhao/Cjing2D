#include "player\grabbingState.h"
#include "player\pushingState.h"
#include "entity\player.h"
#include "movements\movement.h"
#include "player\playerSprite.h"
#include "game\equipment.h"
#include "game\weapon.h"
#include "game\gameCommands.h"
#include "game\game.h"

GrabbingState::GrabbingState(Entity & entity):
	PlayerState(entity, "Grabbing")
{
}

void GrabbingState::Start(EntityState & state)
{
	PlayerState::Start(state);
	auto& player = dynamic_cast<Player&>(GetEntity());

	// 根据状态获取方向
	Direction4 dir = state.GetWantedDirection4();
	player.GetPlayerSprites().SetAnimationDirection4(dir);

	// 设置为移动前的方向，不是鼠标方向
	auto& movement = player.GetMovement();
	if (movement != nullptr)
	{
		Direction4 direction = static_cast<Direction4>(movement->GetDirection());
		player.GetPlayerSprites().SetAnimationDirection4(direction);
	}

	// 隐藏武器
	auto& weapon = player.GetEquipment().GetCurWeapon();
	if (weapon != nullptr)
		weapon->SetVisible(false);

	GetPlayerSprites().SetGrabbingAnimation();
}

void GrabbingState::Stop(EntityState & state)
{
	// 显示武器
	auto& player = dynamic_cast<Player&>(GetEntity());
	auto& weapon = player.GetEquipment().GetCurWeapon();
	if (weapon != nullptr)
	{
		weapon->SetVisible(true);
	}
}

void GrabbingState::Update()
{
	if (IsSuspended())
		return;

	// 如果不按住交互键则恢复为原来的状态
	auto& player = dynamic_cast<Player&>(GetEntity());
	auto& gameCommand = GetGame().GetGameCommands();
	if (!gameCommand.IsGameCommandPressed(GameCommand::GAME_COMMAND_INTERACT))
	{
		player.SetNormalState();
		return;
	}

	int wantedDirection = gameCommand.GetWantedDirection8();
	int spriteDirection = GetPlayerSprites().GetAnimationDirection8();
	if (wantedDirection == spriteDirection)
	{
		player.SetState(new PushingState(player));
		return;
	}
}

bool GrabbingState::IsCanHurt() const
{
	return true;
}
