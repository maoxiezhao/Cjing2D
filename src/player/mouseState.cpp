#include "mouseState.h"
#include "entity\entity.h"
#include "entity\player.h"
#include "player\playerSprite.h"
#include "core\inputEvent.h"
#include "utils\geometry.h"

MouseState::MouseState(Entity & entity):
	MovementState(entity, "MouseMovement"),
	mFacingDegree(0.0f)
{
}

/**
*	\brief 该状态下相应交互键
*
*	这里应注意overlap和facing会以overlap为优先
*/
void MouseState::NotifyCommandInteractPressed()
{
	Player& player = dynamic_cast<Player&>(GetEntity());
	Entity* overlapEntity = player.GetOverlapEntity();
	bool entityInteraction = false;
	if (overlapEntity != nullptr && overlapEntity->IsFosused())
	{
		overlapEntity->NotifyCommandInteractPressed(player);
	}
	else
	{
		// 当overlap不存在时才考虑处理facingEntity
		Entity* facingEntity = player.GetFacingEntity();
		if (facingEntity != nullptr && facingEntity->IsFosused())
		{
			facingEntity->NotifyCommandInteractPressed(player);
		}
	}
}

void MouseState::Start(EntityState & state)
{
	MovementState::Start(state);

	// 设置player关闭command控制位置
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.SetBindDirectoinByGameCommand(false);
}

void MouseState::Stop(EntityState & state)
{
	MovementState::Stop(state);

	// 设置player开启command控制位置
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.SetBindDirectoinByGameCommand(true);
}

/**
*	\brief mouseState刷新函数
*
*	获取当前鼠标坐标，设置对应的方向
*/
void MouseState::Update()
{
	MovementState::Update();

	auto& player = dynamic_cast<Player&>(GetEntity());
	const Point2& mousePos = InputEvent::GetMousePos();
	const Point2& playerPos = player.GetScreenPos();

	// 角度偏移 4方向偏移
	double angle = Geometry::GetAngle(playerPos, mousePos);
	mFacingDegree = (float)angle;
	//if (Geometry::PI_2 - angle <= Geometry::PI_OVER_4)
	//{
	//	angle = Geometry::PI_2 - angle;
	//}
	//angle += Geometry::PI_OVER_4;

	// 角度偏移 2方向
	if (Geometry::PI_2 - angle <= Geometry::PI_OVER_2)
		angle = Geometry::PI_2 - angle;
	angle += Geometry::PI_OVER_2;

	Direction4 direction = static_cast<Direction4>(
		(int)(angle / Geometry::PI_OVER_2));

	player.GetPlayerSprites().SetAnimationDirection4(direction);
}

float MouseState::GetFacingDegree() const
{
	return Geometry::Degree(mFacingDegree);
}

bool MouseState::CanAttack() const
{
	return true;
}

bool MouseState::IsCanHurt() const
{
	return true;
}
