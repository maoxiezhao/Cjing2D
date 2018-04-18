#include "playerState.h"
#include "entity\player.h"
#include "player\playerSprite.h"
#include "game\equipment.h"

PlayerState::PlayerState(Entity & entity):
	EntityState(entity, "PlayerState")
{
}

Player & PlayerState::GetPlayer()
{
	auto& player = dynamic_cast<Player&>(GetEntity());
	return player;
}

const Player & PlayerState::GetPlayer() const
{
	const auto& player = dynamic_cast<const Player&>(GetEntity());
	return player;
}

PlayerSprite & PlayerState::GetPlayerSprites()
{
	return GetPlayer().GetPlayerSprites();
}

Equipment & PlayerState::GetEquipment()
{
	return GetPlayer().GetEquipment();
}

void PlayerState::NotifyCommandAttackPressed()
{
	auto& player = GetPlayer();
	if (player.CanAttack())
	{
		player.Attack();
	}
}

void PlayerState::NotifyCommandSwitchPressed()
{
}
