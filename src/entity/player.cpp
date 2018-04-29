#include"entity\player.h"
#include"entity\entities.h"
#include"game\map.h"
#include"game\equipment.h"
#include"player\freeState.h"
#include"player\mouseState.h"
#include"player\playerSprite.h"
#include"core\logger.h"
#include"gui\core\uiRender.h"
#include"game\weapon.h"

Player::Player(Equipment & equipment) :
	Entity("", "", { 50, 50 }, {26, 20}, 0),	// testing data
	mPlayerSprites(nullptr),
	mEquipment(equipment),
	mNormalWalkingSpeed(100),
	mCurWalkingSpeed(100),
	mShiftSpeed(300),
	mIsBindDirectionByGameCommand(true)
{
	SetOrigin({ -3, -22 });
	SetDrawOnYOrder(true);

	// ���õ�ǰplayer sprites
	PlayerSprite* playerSprite = new PlayerSprite(*this);
	if (playerSprite == nullptr)
	{
		Debug::Error("Failed to create player sprites.");
	}
	mPlayerSprites = std::unique_ptr<PlayerSprite>(playerSprite);

	// ���õ�ǰ״̬
	//auto movementState = std::make_shared<MouseState>(*this);
	SetState(new MouseState(*this));

	mWeapon = std::make_shared<Weapon>("head", equipment);
	mWeapon->Equiped(*this);
}

void Player::Update()
{
	// sprite update
	mPlayerSprites->Update();

	// movement update
	GetMovement()->Update();

	// equipment update
	GetEquipment().Update();

	// state update
	GetState().Update();

	// weapon update
	mWeapon->Update();

}

/**
*	\brief ���ƽ�ɫ
*
*	���ڽ�ɫ�Ļ�������Ҫ���ƣ�
*/
void Player::Draw()
{
	// now testing data
	Debug::CheckAssertion(mPlayerSprites != nullptr, "Player sprites is null.");

	//DrawDebugBounding();

	mPlayerSprites->Draw();

	// ԭ����Entity::Draw()ע���ˣ���������ʱ����ԭ����ʹ��
	Entity::Draw();

#if defined CJING_DEBUG
	std::ostringstream oss;
	oss << "player pos: x " << GetPos().x << "  y " << GetPos().y;
	gui::UIRender::renderDebugString(oss.str());
#endif
}

void Player::Initalized()
{
}

void Player::PlaceOnMap(Map & map)
{
	auto sharedPlayer = std::dynamic_pointer_cast<Entity>(this->shared_from_this());
	map.GetEntities().AddEntity(sharedPlayer);
}

/**
*	\brief ��⵱ǰλ��
*
*	������⵱ǰfacing entity����⵱ǰground���Σ����
*	��ǰ������entity����ײ
*/
void Player::CheckPosition()
{
	SetFacingEntity(nullptr);
	SetOverlapEntity(nullptr);
	CheckCollisionWithEntities();
}

/**
*	\brief ��ȡ��ǰ�����ٶ�
*/
int Player::GetWalkingSpeed() const
{
	return mCurWalkingSpeed;
}

/**
*	\brief ��ȡ��ǰ˲���ٶ�
*/
int Player::GetShiftSpeed() const
{
	return mShiftSpeed;
}

PlayerSprite & Player::GetPlayerSprites()
{
	return *mPlayerSprites;
}

EntityType Player::GetEntityType() const
{
	return EntityType::PLAYRE;
}

/**
*	\brief ����player�ķ�����mouse���ƶ�����ȷ��
*	\param binded trueʱ�����󶨣�falseʱ��ʹ�ü��̰�
*/
void Player::SetBindDirectoinByGameCommand(bool binded)
{
	mIsBindDirectionByGameCommand = binded;
}

/**
*	\brief ��ȡsprite�ķ���
*	\return ��8������ʽ����
*/
Direction8 Player::GetDirection8() const
{
	return mPlayerSprites->GetAnimationDirection8();
}

/**
*	\brief �ܷ񹥻����ж�
*/
bool Player::CanAttack() const
{
	return true;

	bool result = GetState().CanAttack() &&
		GetEquipment().HasCurWeapon();
	return result;
}

Equipment & Player::GetEquipment()
{
	return mEquipment;
}

const Equipment & Player::GetEquipment() const
{
	return mEquipment;
}

float Player::GetFacingDegree() const
{
	return GetState().GetFacingDegree();
}

bool Player::IsObstacle(Entity & entity) const
{
	return entity.IsObstaclePlayer();
}

bool Player::IsObstacleEnemy() const
{
	return true;
}

/**
*	\brief ���й�����Ϊ
*/
void Player::Attack()
{
	mWeapon->Attack();

	//if (!CanAttack())
	//{
	//	Debug::Warning("Can not attack now.");
	//	return;
	//}

	//// �����и����⣬��������ֱ����һ�������жϻ���
	//// ��״̬�����жϣ�Ŀǰֱ�����������ж�
	//auto& curWeapon = GetEquipment().GetCurWeapon();
	//curWeapon.Attack();

	
}

/**
*	\brief ��Ӧ�ƶ��ı�
*
*	��Ҫ�������õ�ǰ����
*/
void Player::NotifyMovementChanged()
{
	// ��ǰ��animation directions�Ƿ���ܵ�gameCommandӰ��
	if (mIsBindDirectionByGameCommand)
	{
		Direction8 wantedDirection = GetState().GetWantedDirection8();
		if (wantedDirection != Direction8::DIRECTION8_NONE)
		{
			Direction4 animationDirection = mPlayerSprites->GetAnimationDirection4();
			Direction8 realDirection = wantedDirection;	// GetRealDirection();
			Direction4 direction = mPlayerSprites->GetAnimationDirection4(wantedDirection, realDirection);

			// ���animation����������ƶ�����ͬ�������ö�Ӧ����
			if (direction != animationDirection)
			{
				mPlayerSprites->SetAnimationDirection4(direction);
			}
		}
	}
	// ��Ӧ�����ö����Ĳ�����ͣ
	GetState().NotifyMovementChanged();	
}

/**
*	\brief ��Ӧλ�øı�
*/
void Player::NotifyPositonChanged()
{
	if (IsOnMap())
	{
		GetMap().GetEntities().NotifyEntityRectChanged(*this);
	}
	CheckPosition();
}

/**
*	\brief ��ӦfacingEntity�ĸı�,�����ø�entityΪfocused
*/
void Player::NotifyFacingEntityChanged(Entity * entity)
{
	auto facingEntity = GetFacingEntity();
	if (facingEntity != entity)
	{
		// old entityʧȥ����
		if (facingEntity != nullptr)
		{
			facingEntity->SetFocused(false);
		}
		
		// new entityʰȡ����
		if (entity != nullptr)
		{
			entity->SetFocused(true);
		}
	}
}

/**
*	\brief ��ӦoverlapEntity�ĸı�,�����ø�entityΪfocused
*/
void Player::NotifyOverlapEntityChanged(Entity * entity)
{
	auto overlapEntity = GetOverlapEntity();
	if (overlapEntity != entity)
	{
		// old entityʧȥ����
		if (overlapEntity != nullptr)
		{
			overlapEntity->SetFocused(false);
		}

		// new entityʰȡ����
		if (entity != nullptr)
		{
			entity->SetFocused(true);
		}
	}
}

/**
*	\brief ����˷�����ײ�¼�,�����ﴦ���˺���Ϊ��
*/
void Player::NotifyCollisionWithEnemy(Enemy & enemy)
{
	Logger::Info("Notify Collison.");
}
