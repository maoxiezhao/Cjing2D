#include "playerSprite.h"
#include "game\map.h"

namespace{
	const std::string walkAnimationName = "walking";
	const std::string stopAnimationName = "stopping";

	// 4方向动画方向，用于多个方向产生时确定
	// 真正移动的方向,仅用在4方向行走图
	Direction4 animationRealDirections[8][2] = {
		{ Direction4::DIRECTION4_RIGHT, Direction4::DIRECTION4_NONE},	// right
		{ Direction4::DIRECTION4_RIGHT, Direction4::DIRECTION4_UP },	// right-up
		{ Direction4::DIRECTION4_UP,    Direction4::DIRECTION4_NONE },	// up
		{ Direction4::DIRECTION4_LEFT,  Direction4::DIRECTION4_UP },	// up-left
		{ Direction4::DIRECTION4_LEFT,  Direction4::DIRECTION4_NONE },	// left
		{ Direction4::DIRECTION4_DOWN,  Direction4::DIRECTION4_LEFT },	// left-down
		{ Direction4::DIRECTION4_DOWN,  Direction4::DIRECTION4_NONE },	// down
		{ Direction4::DIRECTION4_RIGHT, Direction4::DIRECTION4_DOWN }	// down-right
	};
}

PlayerSprite::PlayerSprite(Player & player):
	mPlayer(player),
	mIsFourDirecitonSprite(true),
	mBodySprite(nullptr),
	mShadowSprite(nullptr)
{
	BuildSprites();
}

/**
*	\brief 构建player sprites
*/
void PlayerSprite::BuildSprites()
{
	const Point2& pos = mPlayer.GetOrigin();

	// 创建影子的sprite
	mShadowSprite = mPlayer.CreateAnimationSprite("entities/shadow", "big");
	mShadowSprite->SetAnchorFloat(0.5f, 0.85f);

	// 创建身体的sprite
	mBodySprite = mPlayer.CreateAnimationSprite("players/player", walkAnimationName); 
	mBodySprite->SetCurrDirection(0);

	SetAnimationDirection4(Direction4::DIRECTION4_DOWN);
	SetStopNormalAnimation();
}

/**
*	\brief 刷新player的sprites
*/
void PlayerSprite::Update()
{
	mBodySprite->Update();
}

/**
*	\brief 绘制player的sprites
*
*	依次绘制bodySprite，shadowSprite
*/
void PlayerSprite::Draw()
{
	auto& map = mPlayer.GetMap();
	const Point2& pos = mPlayer.GetPos();
	const Point2& ltPos = mPlayer.GetLeftTopPos();
	const Size& size = mPlayer.GetSize();

	mShadowSprite->SetPos({ ltPos.x + size.width/2, ltPos.y + size.height});
	map.DrawOnMap(*mShadowSprite);

	mBodySprite->SetPos(pos);

	map.DrawOnMap(*mBodySprite);
}

/**
*	\brief 设置当前动画为步行动画
*/
void PlayerSprite::SetWalkingNormalAnimation()
{
	mBodySprite->SetCurrAnimation(walkAnimationName);
	mBodySprite->StartAnimation();
}

/**
*	\brief 设置当前动画为停止站立动画
*/
void PlayerSprite::SetStopNormalAnimation()
{
	mBodySprite->SetCurrAnimation(stopAnimationName);
	mBodySprite->StartAnimation();
}

/**
*	\brief 设置当前动画方向,此时精灵应是4方向精灵图
*/
void PlayerSprite::SetAnimationDirection4(Direction4 direction)
{
	mBodySprite->SetCurrDirection(static_cast<int>(direction));
}

/**
*	\brief 设置当前动画方向,此时精灵应是8方向精灵图
*/
void PlayerSprite::SetAnimationDirection8(Direction8 direction)
{
	mBodySprite->SetCurrDirection(static_cast<int>(direction));
}

/**
*	\brief 获取当前动画的方向4方向
*/
Direction4 PlayerSprite::GetAnimationDirection4() const
{
	return static_cast<Direction4>(mBodySprite->GetCurrDirection());
}

/**
*	\brief 获取当前动画的方向8方向
*/
Direction8 PlayerSprite::GetAnimationDirection8() const
{
	return static_cast<Direction8>(mBodySprite->GetCurrDirection() * 2);
}

/**
*	\brief 获取当前动画的真实方向
*
*	根据当前方向动画方向和移动方向设置最终移动方向
*/						 
Direction4 PlayerSprite::GetAnimationDirection4(Direction8 wantedDirection, Direction8 realDirection) const
{
	Direction4 direction = Direction4::DIRECTION4_NONE;
	if (wantedDirection != Direction8::DIRECTION8_NONE)
	{
		int numDirection = static_cast<int>(wantedDirection);
		// 单方向则直接返回
		if (numDirection % 2 == 0)
		{
			direction = static_cast<Direction4>(numDirection / 2);
		}
		//else if (animationRealDirections[numDirection][1] == realDirection)
		//{	// 复合方向，但真正移动的方向取其中一个（某些角落处即便复合
		//	// 运动做不了，依旧可以移动）
		//	direction = animationRealDirections[numDirection][1];
		//}
		else
		{	// 否则返回animationRealDirections定义的优先级方向
			direction = animationRealDirections[numDirection][0];
		}
	}
	return direction;
}

