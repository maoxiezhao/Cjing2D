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
*	\brief ��״̬����Ӧ������
*
*	����Ӧע��overlap��facing����overlapΪ����
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
		// ��overlap������ʱ�ſ��Ǵ���facingEntity
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

	// ����player�ر�command����λ��
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.SetBindDirectoinByGameCommand(false);
}

void MouseState::Stop(EntityState & state)
{
	MovementState::Stop(state);

	// ����player����command����λ��
	auto& player = dynamic_cast<Player&>(GetEntity());
	player.SetBindDirectoinByGameCommand(true);
}

/**
*	\brief mouseStateˢ�º���
*
*	��ȡ��ǰ������꣬���ö�Ӧ�ķ���
*/
void MouseState::Update()
{
	MovementState::Update();

	auto& player = dynamic_cast<Player&>(GetEntity());
	const Point2& mousePos = InputEvent::GetMousePos();
	const Point2& playerPos = player.GetScreenPos();

	// �Ƕ�ƫ�� 4����ƫ��
	double angle = Geometry::GetAngle(playerPos, mousePos);
	mFacingDegree = (float)angle;
	//if (Geometry::PI_2 - angle <= Geometry::PI_OVER_4)
	//{
	//	angle = Geometry::PI_2 - angle;
	//}
	//angle += Geometry::PI_OVER_4;

	// �Ƕ�ƫ�� 2����
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
