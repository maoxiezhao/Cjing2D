#ifndef _ANIMATION_SPRITE_H_
#define _ANIMATION_SPRITE_H_

#include"common\common.h"
#include"game\sprite.h"
#include"game\animationSet.h"

/**
*	\brief 动画精灵，可以加载动画文件，播放精灵帧动画
*/
class AnimationSprite : public Sprite
{
public:
	AnimationSprite(const string& id);
	~AnimationSprite();

	// system
	virtual void Update();
	virtual void Draw();
	virtual bool IsAnimationed()const;

	bool IsFrameChanged()const;

	// status


private:
	void UpdateFramedChanged();

	uint32_t mFrameDelay;
	uint32_t mNextFrameDate;
	int      mFrameLoop;
	int      mCurrFrame;
	int      mFrameNum;
	bool     mFrameChanged;
};


#endif