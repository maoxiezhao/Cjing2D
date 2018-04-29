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

	// 设置当前player sprites
	PlayerSprite* playerSprite = new PlayerSprite(*this);
	if (playerSprite == nullptr)
	{
		Debug::Error("Failed to create player sprites.");
	}
	mPlayerSprites = std::unique_ptr<PlayerSprite>(playerSprite);

	// 设置当前状态
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
*	\brief 绘制角色
*
*	对于角色的绘制依次要绘制：
*/
void Player::Draw()
{
	// now testing data
	Debug::CheckAssertion(mPlayerSprites != nullptr, "Player sprites is null.");

	//DrawDebugBounding();

	mPlayerSprites->Draw();

	// 原来把Entity::Draw()注掉了？？？，暂时忘了原因，先使用
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
*	\brief 检测当前位置
*
*	包括检测当前facing entity，检测当前ground地形，检测
*	当前与其他entity的碰撞
*/
void Player::CheckPosition()
{
	SetFacingEntity(nullptr);
	SetOverlapEntity(nullptr);
	CheckCollisionWithEntities();
}

/**
*	\brief 获取当前步行速度
*/
int Player::GetWalkingSpeed() const
{
	return mCurWalkingSpeed;
}

/**
*	\brief 获取当前瞬移速度
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
*	\brief 设置player的方向由mouse的移动坐标确定
*	\param binded true时则鼠标绑定，false时则使用键盘绑定
*/
void Player::SetBindDirectoinByGameCommand(bool binded)
{
	mIsBindDirectionByGameCommand = binded;
}

/**
*	\brief 获取sprite的方向
*	\return 以8方向形式返回
*/
Direction8 Player::GetDirection8() const
{
	return mPlayerSprites->GetAnimationDirection8();
}

/**
*	\brief 能否攻击的判定
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
*	\brief 进行攻击行为
*/
void Player::Attack()
{
	mWeapon->Attack();

	//if (!CanAttack())
	//{
	//	Debug::Warning("Can not attack now.");
	//	return;
	//}

	//// 这里有个问题，是在这里直接做一个攻击判断还是
	//// 在状态里做判断，目前直接在这里做判断
	//auto& curWeapon = GetEquipment().GetCurWeapon();
	//curWeapon.Attack();

	
}

/**
*	\brief 响应移动改变
*
*	需要重新设置当前方向
*/
void Player::NotifyMovementChanged()
{
	// 当前的animation directions是否会受到gameCommand影响
	if (mIsBindDirectionByGameCommand)
	{
		Direction8 wantedDirection = GetState().GetWantedDirection8();
		if (wantedDirection != Direction8::DIRECTION8_NONE)
		{
			Direction4 animationDirection = mPlayerSprites->GetAnimationDirection4();
			Direction8 realDirection = wantedDirection;	// GetRealDirection();
			Direction4 direction = mPlayerSprites->GetAnimationDirection4(wantedDirection, realDirection);

			// 如果animation方向和期望移动方向不同，则设置对应方向
			if (direction != animationDirection)
			{
				mPlayerSprites->SetAnimationDirection4(direction);
			}
		}
	}
	// 响应中设置动画的播放暂停
	GetState().NotifyMovementChanged();	
}

/**
*	\brief 响应位置改变
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
*	\brief 响应facingEntity的改变,并设置该entity为focused
*/
void Player::NotifyFacingEntityChanged(Entity * entity)
{
	auto facingEntity = GetFacingEntity();
	if (facingEntity != entity)
	{
		// old entity失去焦点
		if (facingEntity != nullptr)
		{
			facingEntity->SetFocused(false);
		}
		
		// new entity拾取焦点
		if (entity != nullptr)
		{
			entity->SetFocused(true);
		}
	}
}

/**
*	\brief 响应overlapEntity的改变,并设置该entity为focused
*/
void Player::NotifyOverlapEntityChanged(Entity * entity)
{
	auto overlapEntity = GetOverlapEntity();
	if (overlapEntity != entity)
	{
		// old entity失去焦点
		if (overlapEntity != nullptr)
		{
			overlapEntity->SetFocused(false);
		}

		// new entity拾取焦点
		if (entity != nullptr)
		{
			entity->SetFocused(true);
		}
	}
}

/**
*	\brief 与敌人发生碰撞事件,在这里处理伤害行为？
*/
void Player::NotifyCollisionWithEnemy(Enemy & enemy)
{
	Logger::Info("Notify Collison.");
}
