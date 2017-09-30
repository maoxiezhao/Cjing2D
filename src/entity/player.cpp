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

	// ���õ�ǰplayer sprites
	PlayerSprite* playerSprite = new PlayerSprite(*this);
	if (playerSprite == nullptr)
	{
		Debug::Error("Failed to create player sprites.");
	}
	mPlayerSprites = std::unique_ptr<PlayerSprite>(playerSprite);

	// ���õ�ǰ״̬
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
*	\brief ���ƽ�ɫ
*
*	���ڽ�ɫ�Ļ�������Ҫ���ƣ�
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
*	\brief ��ȡ��ǰ�����ٶ�
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
*	\brief ����player�ķ�����mouse���ƶ�����ȷ��
*	\param binded trueʱ�����󶨣�falseʱ��ʹ�ü��̰�
*/
void Player::SetBindDirectoinByGameCommand(bool binded)
{
	mIsBindDirectionByGameCommand = binded;
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
		Direction8 wantedDirection = GetState()->GetWantedDirection8();
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
	GetState()->NotifyMovementChanged();	
}
