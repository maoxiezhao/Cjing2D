#include"entity\player.h"
#include"entity\entities.h"
#include"entity\destination.h"
#include"game\map.h"
#include"game\equipment.h"
#include"game\weapon.h"
#include"player\freeState.h"
#include"player\mouseState.h"
#include"player\playerSprite.h"
#include"core\logger.h"
#include"gui\core\uiRender.h"
#include"lua\luaContext.h"

Player::Player(Equipment & equipment) :
	Entity("player", "", { 50, 50 }, {26, 20}, 0),	// testing data
	mPlayerSprites(nullptr),
	mEquipment(equipment),
	mNormalWalkingSpeed(100),
	mCurWalkingSpeed(100),
	mShiftSpeed(300),
	mIsBindDirectionByGameCommand(true)
{
	SetOrigin({ -3, -22 });
	SetDrawOnYOrder(true);

	// player�Ƿ���Ҫ����collision_mode
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);

	// ���õ�ǰplayer sprites
	PlayerSprite* playerSprite = new PlayerSprite(*this);
	if (playerSprite == nullptr)
		Debug::Error("Failed to create player sprites.");
	
	mPlayerSprites = std::unique_ptr<PlayerSprite>(playerSprite);

	// ���õ�ǰ״̬
	SetNormalState();
}

void Player::Update()
{
	// sprite update
	mPlayerSprites->Update();

	// removed sprite clear
	ClearRemovedSprite();

	// movement update
	GetMovement()->Update();

	// equipment update
	GetEquipment().Update();

	// state update
	GetState().Update();

	// lua update if exists
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnUpdate",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, *this);
		return 1;
	});
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
	Entity::Initalized();
}

void Player::PlaceOnMap(Map & map)
{
	// ��ǰ��ͼ��ͬ
	if (IsOnMap() && &GetMap() == &map)
		return;

	// ˢ���µ�ǰ״̬
	mEquipment.NotifyPlayerEnter(*this);

	// ��ӵ���ͼ��
	auto sharedPlayer = std::dynamic_pointer_cast<Entity>(this->shared_from_this());
	map.GetEntities().AddEntity(sharedPlayer);

	// ����λ�úͷ���
	auto destination = map.GetDestination();
	if (destination)
	{
		SetPos(destination->GetPos());
	}

	SetNormalState();
	CheckPosition();
}

/**
*	\brief ��⵱ǰλ��
*
*	������⵱ǰfacing entity����⵱ǰground���Σ����
*	��ǰ������entity����ײ
*/
void Player::CheckPosition()
{
	if (!IsOnMap())
		return;

	SetFacingEntity(nullptr);
	SetOverlapEntity(nullptr);

	if (IsHaveCollision())
		CheckCollisionFromEntities();

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

const string Player::GetLuaObjectName() const
{
	return LuaContext::module_player_name;
}

/**
*	\brief ���й�����Ϊ
*/
void Player::Attack()
{
	if (!CanAttack())
	{
		Debug::Warning("Can not attack now.");
		return;
	}
	//// �����и����⣬��������ֱ����һ�������жϻ���
	//// ��״̬�����жϣ�Ŀǰֱ�����������ж�
	auto& curWeapon = GetEquipment().GetCurWeapon();
	curWeapon->Attack();
}

void Player::SetNormalState()
{
	SetState(new MouseState(*this));
}

void Player::NotifyAttack()
{
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
		GetMap().GetEntities().NotifyEntityRectChanged(*this);
	
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
*	\brief ��ײentity��ӦCollisionWithPlayer��Ϊ�˱����ظ�Notify��Ŀǰ��Bullet��Ӧplayer
*/
void Player::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	otherEntity.NotifyCollisionWithPlayer(*this);
}

/**
*	\brief ����˷�����ײ�¼�,�����ﴦ���˺���Ϊ��
*/
void Player::NotifyCollisionWithEnemy(Enemy & enemy)
{
	Logger::Info("Notify Collison.");
}

void Player::NotifyAfterCreated()
{
	// ��Ӧ��ҽ����¼�
	// ִ��map Enter
	GetLuaContext()->EnterPlayer(*this);
}

void Player::NotifyBeRemoved()
{
	GetLuaContext()->LeavePlayer(*this);
}
