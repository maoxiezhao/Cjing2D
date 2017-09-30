#include"entity\player.h"
#include"entity\entities.h"
#include"game\map.h"
#include"player\freeState.h"
#include"player\mouseState.h"
#include"player\playerSprite.h"

Player::Player():
	Entity("", { 200, 200 }, {30, 30}, 0),	// testing data
	mPlayerSprites(nullptr),
	mNormalWalkingSpeed(100),
	mCurWalkingSpeed(100),
	mIsBindDirectionByGameCommand(true)
{
	SetOrigin({ -5, -10 });

	// 设置当前player sprites
	PlayerSprite* playerSprite = new PlayerSprite(*this);
	if (playerSprite == nullptr)
	{
		Debug::Error("Failed to create player sprites.");
	}
	mPlayerSprites = std::unique_ptr<PlayerSprite>(playerSprite);

	// 设置当前状态
	auto movementState = std::make_shared<MouseState>(*this);
	SetState(movementState);
}

void Player::Update()
{
	// sprite update
	mPlayerSprites->Update();

	// movement update
	GetMovement()->Update();

	// state update
	GetState()->Update();
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

//	DrawDebugBounding();

	mPlayerSprites->Draw();
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
*	\brief 获取当前步行速度
*/
int Player::GetWalkingSpeed() const
{
	return mCurWalkingSpeed;
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
*	\brief 响应移动改变
*
*	需要重新设置当前方向
*/
void Player::NotifyMovementChanged()
{
	// 当前的animation directions是否会受到gameCommand影响
	if (mIsBindDirectionByGameCommand)
	{
		Direction8 wantedDirection = GetState()->GetWantedDirection8();
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
	GetState()->NotifyMovementChanged();	
}
