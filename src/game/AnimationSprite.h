#ifndef _ANIMATION_SPRITE_H_
#define _ANIMATION_SPRITE_H_

#include"common\common.h"
#include"game\sprite.h"
#include"game\spriteAnimation.h"


/**
*	\brief 动画精灵，可以加载动画文件，播放精灵帧动画
*/
class AnimationSprite : public Sprite
{
public:
	AnimationSprite(const string& id);
};


#endif