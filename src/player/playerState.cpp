#include "playerState.h"
#include "entity\player.h"
#include "player\playerSprite.h"
#include "game\equipment.h"
#include "game\game.h"

PlayerState::PlayerState(Entity & entity, const std::string& name):
	EntityState(entity, name)
{
}

void PlayerState::Update()
{
	auto& gameCommand = GetPlayer().GetGame().GetGameCommands();
	if (gameCommand.IsGameCommandPressed(GameCommand::GAME_COMMAND_ATTACK))
	{
		NotifyCommandAttackPressed();
	}
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
