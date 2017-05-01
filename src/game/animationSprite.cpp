#include"animationSprite.h"

AnimationSprite::AnimationSprite(const string & id)
{
}

AnimationSprite::~AnimationSprite()
{
}

void AnimationSprite::Update()
{
}

/**
*	\brief 绘制动画精灵
*
*   绘制时需要考虑是否发生了帧变换
*/
void AnimationSprite::Draw()
{
	if (IsFrameChanged())
		UpdateFramedChanged();

	Sprite::Draw();
}

/**
*	\brief 是否是动画精灵，AnimationSprite必然返回false;
*/
bool AnimationSprite::IsAnimationed() const
{
	return true;
}

/**
*	\brief 是否发生帧变换
*/
bool AnimationSprite::IsFrameChanged() const
{
	return mFrameChanged;
}

/**
*	\brief 更新变换的帧
*/
void AnimationSprite::UpdateFramedChanged()
{
}
