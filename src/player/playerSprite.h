#pragma once

#include"game\animationSprite.h"
#include"game\gameCommands.h"
#include"entity\player.h"

/**
*	\brief player sprite
*
*	player sprite����ǰPlayerd�ľ�����ʾ��Player sprite��
*	һϵ�е�sprite��ƴװ����������߸���sprite����ʾ������Ӧ
*	��������ʾ
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

	bool mIsFourDirecitonSprite;		/** �þ����Ƿ���4����Ĭ����4���� */
	
	/**** **** sprite **** ****/
	AnimationSpritePtr mBodySprite;		/** ����ľ��� */
	AnimationSpritePtr mShadowSprite;
};

inline bool PlayerSprite::IsFourDirection()const
{
	return mIsFourDirecitonSprite;
}