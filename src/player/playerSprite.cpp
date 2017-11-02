#include "playerSprite.h"
#include "game\map.h"

namespace{
	const std::string walkAnimationName = "walking";
	const std::string stopAnimationName = "stopping";

	// 4���򶯻��������ڶ���������ʱȷ��
	// �����ƶ��ķ���,������4��������ͼ
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
*	\brief ����player sprites
*/
void PlayerSprite::BuildSprites()
{
	const Point2& pos = mPlayer.GetOrigin();

	// ����Ӱ�ӵ�sprite
	mShadowSprite = mPlayer.CreateAnimationSprite("entities/shadow", "big");
	mShadowSprite->SetAnchorFloat(0.5f, 0.85f);

	// ���������sprite
	mBodySprite = mPlayer.CreateAnimationSprite("players/player", walkAnimationName); 
	mBodySprite->SetCurrDirection(0);

	SetAnimationDirection4(Direction4::DIRECTION4_DOWN);
	SetStopNormalAnimation();
}

/**
*	\brief ˢ��player��sprites
*/
void PlayerSprite::Update()
{
	mBodySprite->Update();
}

/**
*	\brief ����player��sprites
*
*	���λ���bodySprite��shadowSprite
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
*	\brief ���õ�ǰ����Ϊ���ж���
*/
void PlayerSprite::SetWalkingNormalAnimation()
{
	mBodySprite->SetCurrAnimation(walkAnimationName);
	mBodySprite->StartAnimation();
}

/**
*	\brief ���õ�ǰ����Ϊֹͣվ������
*/
void PlayerSprite::SetStopNormalAnimation()
{
	mBodySprite->SetCurrAnimation(stopAnimationName);
	mBodySprite->StartAnimation();
}

/**
*	\brief ���õ�ǰ��������,��ʱ����Ӧ��4������ͼ
*/
void PlayerSprite::SetAnimationDirection4(Direction4 direction)
{
	mBodySprite->SetCurrDirection(static_cast<int>(direction));
}

/**
*	\brief ���õ�ǰ��������,��ʱ����Ӧ��8������ͼ
*/
void PlayerSprite::SetAnimationDirection8(Direction8 direction)
{
	mBodySprite->SetCurrDirection(static_cast<int>(direction));
}

/**
*	\brief ��ȡ��ǰ�����ķ���4����
*/
Direction4 PlayerSprite::GetAnimationDirection4() const
{
	return static_cast<Direction4>(mBodySprite->GetCurrDirection());
}

/**
*	\brief ��ȡ��ǰ�����ķ���8����
*/
Direction8 PlayerSprite::GetAnimationDirection8() const
{
	return static_cast<Direction8>(mBodySprite->GetCurrDirection() * 2);
}

/**
*	\brief ��ȡ��ǰ��������ʵ����
*
*	���ݵ�ǰ���򶯻�������ƶ��������������ƶ�����
*/						 
Direction4 PlayerSprite::GetAnimationDirection4(Direction8 wantedDirection, Direction8 realDirection) const
{
	Direction4 direction = Direction4::DIRECTION4_NONE;
	if (wantedDirection != Direction8::DIRECTION8_NONE)
	{
		int numDirection = static_cast<int>(wantedDirection);
		// ��������ֱ�ӷ���
		if (numDirection % 2 == 0)
		{
			direction = static_cast<Direction4>(numDirection / 2);
		}
		//else if (animationRealDirections[numDirection][1] == realDirection)
		//{	// ���Ϸ��򣬵������ƶ��ķ���ȡ����һ����ĳЩ���䴦���㸴��
		//	// �˶������ˣ����ɿ����ƶ���
		//	direction = animationRealDirections[numDirection][1];
		//}
		else
		{	// ���򷵻�animationRealDirections��������ȼ�����
			direction = animationRealDirections[numDirection][0];
		}
	}
	return direction;
}

