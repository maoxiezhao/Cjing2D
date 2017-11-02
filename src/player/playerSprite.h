#pragma once

#include"game\animationSprite.h"
#include"game\gameCommands.h"
#include"entity\player.h"

/**
*	\brief player sprite
*
*	player sprite管理当前Playerd的精灵显示，Player sprite是
*	一系列的sprite的拼装，该类管理者各个sprite的显示，和响应
*	动画的显示
*/
class PlayerSprite
{
public:
	PlayerSprite(Player& player);

	void BuildSprites();
	void Update();
	void Draw();

	/** Animation control */
	void SetWalkingNormalAnimation();
	void SetStopNormalAnimation();

	/** Status */
	void SetAnimationDirection4(Direction4 direction);
	void SetAnimationDirection8(Direction8 direction);
	Direction4 GetAnimationDirection4()const;
	Direction4 GetAnimationDirection4(Direction8 wantedDirection, Direction8 realDirection)const;
	Direction8 GetAnimationDirection8()const;
	bool IsFourDirection()const;

private:
	Player& mPlayer;

	bool mIsFourDirecitonSprite;		/** 该精灵是否是4方向，默认是4方向 */
	
	/**** **** sprite **** ****/
	AnimationSpritePtr mBodySprite;		/** 身体的精灵 */
	AnimationSpritePtr mShadowSprite;
};

inline bool PlayerSprite::IsFourDirection()const
{
	return mIsFourDirecitonSprite;
}