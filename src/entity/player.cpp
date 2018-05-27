#include"entity\player.h"
#include"entity\entities.h"
#include"entity\destination.h"
#include"entity\block.h"
#include"entity\chest.h"
#include"entity\enemy.h"

#include"game\map.h"
#include"game\equipment.h"
#include"game\weapon.h"
#include"player\freeState.h"
#include"player\mouseState.h"
#include"player\grabbingState.h"
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

	// player是否需要设置collision_mode
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);

	// 设置当前player sprites
	PlayerSprite* playerSprite = new PlayerSprite(*this);
	if (playerSprite == nullptr)
		Debug::Error("Failed to create player sprites.");
	
	mPlayerSprites = std::unique_ptr<PlayerSprite>(playerSprite);

	// 设置当前状态
	SetNormalState();
}

void Player::Update()
{
	// sprite update
	mPlayerSprites->Update();

	// removed sprite clear
	ClearRemovedSprite();

	// movement update
	if(GetMovement() != nullptr)
		GetMovement()->Update();

	// equipment update
	GetEquipment().Update();

	// state update
	GetState().Update();

	// attach update
	UpdateAttachEntities();

	// lua update if exists
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnUpdate",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, *this);
		return 1;
	});
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
	Entity::Initalized();
}

void Player::PlaceOnMap(Map & map)
{
	// 当前地图相同
	if (IsOnMap() && &GetMap() == &map)
		return;

	// 刷新下当前状态
	mEquipment.NotifyPlayerEnter(*this);

	// 添加到地图中
	auto sharedPlayer = std::dynamic_pointer_cast<Entity>(this->shared_from_this());
	map.GetEntities().AddEntity(sharedPlayer);

	// 设置位置和方向
	auto destination = map.GetDestination();
	if (destination)
	{
		SetPos(destination->GetPos());
	}

	SetNormalState();
	CheckPosition();
}

/**
*	\brief 检测当前位置
*
*	包括检测当前facing entity，检测当前ground地形，检测
*	当前与其他entity的碰撞
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

Direction4 Player::GetDirection4() const
{
	auto dir = mPlayerSprites->GetAnimationDirection8();
	return static_cast<Direction4>(dir / 2);
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
	bool result = GetState().CanAttack() &&
		GetEquipment().HasCurWeapon();
	return result;
}

bool Player::IsWeaponAttack() const
{
	return false;
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

Point2 Player::GetFacingPoint() const
{
	Direction4 dir = DIRECTION4_NONE;
	if (GetMovement() != nullptr)
		dir = static_cast<Direction4>( GetMovement()->GetDirection());
	
	return GetPointByDirection(dir);
}

bool Player::IsObstacle(Entity & entity) const
{
	return entity.IsObstaclePlayer();
}

bool Player::IsObstacleEnemy() const
{
	return true;
}

bool Player::IsObstacleBlock() const
{
	return true;
}

bool Player::isObstacleChest() const
{
	return true;
}

const string Player::GetLuaObjectName() const
{
	return LuaContext::module_player_name;
}

/**
*	\brief 进行攻击行为
*/
void Player::Attack()
{
	if (!CanAttack())
	{
		Debug::Warning("Can not attack now.");
		return;
	}
	//// 这里有个问题，是在这里直接做一个攻击判断还是
	//// 在状态里做判断，目前直接在这里做判断
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

void Player::Grabbing()
{
	SetState(new GrabbingState(*this));
}

void Player::StopGrabbing()
{
	GetState().NotifyStopGrabbing();
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

void Player::NotifyMovementFinished()
{
	GetState().NotifyMovementFinished();
}

/**
*	\brief 响应位置改变
*/
void Player::NotifyPositonChanged()
{
	if (IsOnMap())
		GetMap().GetEntities().NotifyEntityRectChanged(*this);
	
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

		// lua notify
		GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyFacingEntityChanged",
			[&](lua_State*l)->int {
			if (entity) {
				GetLuaContext()->PushUserdata(l, *this);
				return 1;
			}
			return 0;
		});
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
*	\brief 碰撞entity响应CollisionWithPlayer，为了避免重复Notify，目前仅Bullet响应player
*/
void Player::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	otherEntity.NotifyCollisionWithPlayer(*this);
}

/**
*	\brief 与敌人发生碰撞事件,在这里处理伤害行为？
*/
void Player::NotifyCollisionWithEnemy(Enemy & enemy)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyCollisionEnemy",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, enemy);
		return 1;
	});
}

void Player::NotifyCollisionWithBlock(Block & block)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyCollisionBlock",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, block);
		return 1;
	});
}

void Player::NotifyCollisionWithChest(Chest & chest)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyCollisionChest",
		[&](lua_State*l)->int {
		GetLuaContext()->PushUserdata(l, chest);
		return 1;
	});
}

void Player::NotifyAfterCreated()
{
	// 响应玩家进入事件
	// 执行map Enter
	GetLuaContext()->EnterPlayer(*this);
}

void Player::NotifyBeRemoved()
{
	GetLuaContext()->LeavePlayer(*this);
}
