#include "mouseState.h"
#include "entity\player.h"
#include "player\playerSprite.h"
#include "core\inputEvent.h"
#include "utils\geometry.h"

MouseState::MouseState(Entity & entity):
	MovementState(entity)
{
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
	const Point2& playerPos = player.GetPos();

	// 角度偏移
	double angle = Geometry::GetAngle(playerPos, mousePos);
	if (Geometry::PI_2 - angle <= Geometry::PI_OVER_4)
	{
		angle = Geometry::PI_2 - angle;
	}
	angle += Geometry::PI_OVER_4;

	Direction4 direction = static_cast<Direction4>(
		(int)(angle / Geometry::PI_OVER_2));

	player.GetPlayerSprites().SetAnimationDirection4(direction);
}
