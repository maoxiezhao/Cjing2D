#ifndef _ANIMATION_SET_H_
#define _ANIMATION_SET_H_

#include"common\common.h"
#include"game\animationData.h"
#include"game\animation.h"
#include<map>

/**
*	\brief 动画序列集
*/
class AnimationSet
{
public:
	AnimationSet();
	AnimationSet(const string& name);
	~AnimationSet();

	string GetAnimationSetName()const;
	string GetAnimationDefaultName()const;

	// animation
	bool HasAnimation(const string& name)const;
	const Animation& GetAnimation(const string& name)const;
	Animation& GetAnimation(const string& name);
	void AddAnimation(const string&name, const Animation& animation);
	void AddAnimation(const string&name, const AnimationData& animationData);

	// state
	Size GetMaxSize()const;
	Rect GetMaxBoundingBox()const;

private:
	void InitAnimationSetWithFile(const string& name);

	string mAnimationsSetName;
	string mAnimationDefaultName;
	std::map<string, Animation> mAnimations;

	Size mMaxSize;				// 最大大小
	Rect mMaxBoundingBox;		// 最大碰撞盒


};

using AnimationSetPtr = std::shared_ptr<AnimationSet>;

#endif