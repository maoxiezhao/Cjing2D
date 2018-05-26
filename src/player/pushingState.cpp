#include "pushingState.h"
#include "entity\player.h"
#include "movements\movement.h"
#include "player\playerSprite.h"
#include "player\grabbingState.h"
#include "game\gameCommands.h"
#include "game\game.h"
#include "movements\pathMovement.h"

PushingState::PushingState(Entity & entity):
	PlayerState(entity, "Pulling"),
	mPushingDirection(DIRECTION4_NONE),
	mPushingEntity(nullptr),
	mMovement(nullptr)
{
}

void PushingState::Start(EntityState & state)
{
	PlayerState::Start(state);
	mPushingDirection = GetPlayerSprites().GetAnimationDirection4();
}

void PushingState::Stop(EntityState & state)
{
	PlayerState::Stop(state);

	if (IsPushingEntity())
	{
		auto& player = dynamic_cast<Player&>(GetEntity());
		player.StopMovement();
	}
}

void PushingState::Update()
{
	PlayerState::Update();

	if (!IsPushingEntity())
	{
		auto& player = dynamic_cast<Player&>(GetEntity());

		// 如果不是按下指向障碍物方向则返回GrabbingState
		auto& gameCommand = GetGame().GetGameCommands();
		if (gameCommand.GetWantedDirection8() != mPushingDirection * 2)
		{
			if (!gameCommand.IsGameCommandPressed(GameCommand::GAME_COMMAND_INTERACT))
				player.SetNormalState();
			else
				player.SetState(new GrabbingState(player));
		}
		else
		{
			// 当玩家还未推动任意的Entity时则获取FacingEntity并尝试推动
			Entity* entity = player.GetFacingEntity();
			if (entity == nullptr)
			{
				player.SetNormalState();
				return;
			}

			if (entity->StartMoveByPushed(player))
			{
				// 总共移动32pixel
				std::string path = "";
				for (int i = 0; i < 4; i++)
					path += std::to_string(mPushingDirection * 2);

				mMovement = std::make_shared<PathMovement>(path, 32, false, true);
				player.StartMovement(mMovement);
				mPushingEntity = entity;
			}
		}
	}
}

void PushingState::NotifyMovementFinished()
{
	if (IsPushingEntity())
	{
		StopPushingEntity();
	}
}

void PushingState::NotifyStopGrabbing()
{
	StopPushingEntity();
}

Direction4 PushingState::GetWantedDirection4() const
{
	return mPushingDirection;
}

bool PushingState::IsPushingEntity() const
{
	return mPushingEntity != nullptr;
}

/**
*	\brief 停止推动Entity
*/
void PushingState::StopPushingEntity()
{
	auto& player = dynamic_cast<Player&>(GetEntity());

	// 如果存在正在推动的entity则停止
	if (mPushingEntity != nullptr)
	{
		mPushingEntity->StopMoveByPushed();
		player.StopMovement();

		// 需要做一个位置调整，避免玩家被卡在block中
		PlayerPositionAdjust();

		mPushingEntity = nullptr;
		mMovement = nullptr;
	}

	// 如果不是按下指向障碍物方向则返回GrabbingState
	auto& gameCommand = GetGame().GetGameCommands();
	if (!gameCommand.IsGameCommandPressed(GameCommand::GAME_COMMAND_INTERACT))
		player.SetNormalState();
	else
		player.SetState(new GrabbingState(player));

}

/**
*	\brief 移动结束时，玩家可能会有部分点卡在Block的包围盒中，导致无法移动
*	需要根据相对位置，重新移动
*/
void PushingState::PlayerPositionAdjust()
{
	if (mPushingEntity != nullptr)
	{
		auto& player = dynamic_cast<Player&>(GetEntity());
		Point2 pos = player.GetPos();
		switch (mPushingDirection)
		{
		case DIRECTION4_RIGHT:
			pos.x = mPushingEntity->GetPos().x -  player.GetSize().width - 10;
			break;
		case DIRECTION4_UP:
			pos.y = mPushingEntity->GetPos().y + mPushingEntity->GetSize().height + 10;
			break;
		case DIRECTION4_LEFT:
			pos.x = mPushingEntity->GetPos().x + mPushingEntity->GetSize().width + 10;
			break;
		case DIRECTION4_DOWN:
			pos.y = mPushingEntity->GetPos().y - player.GetSize().height - 10;
			break;
		}
		player.SetPos(pos);
	}
}
