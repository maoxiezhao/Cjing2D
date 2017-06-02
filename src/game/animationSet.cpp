#include "animationSet.h"
#include "animationData.h"
#include "core\debug.h"

AnimationSet::AnimationSet()
{
}

AnimationSet::AnimationSet(const string & name):
	mAnimationsSetName(name)
{
	InitAnimationSetWithFile(name);
}

AnimationSet::~AnimationSet()
{
	mAnimations.clear();
}

bool AnimationSet::HasAnimation(const string & name)const
{
	auto animation = mAnimations.find(name);
	return (animation != mAnimations.end());
}

string AnimationSet::GetAnimationSetName() const
{
	return mAnimationsSetName;
}

string AnimationSet::GetAnimationDefaultName() const
{
	return mAnimationDefaultName;
}

/**
*	\brief 返回指定name的animation
*	\return const Animation&
*/
const Animation& AnimationSet::GetAnimation(const string & name)const
{
	Debug::CheckAssertion(HasAnimation(name),"Invalid animation name.");
	return mAnimations.find(name)->second;
}

/**
*	\brief 返回指定name的animation
*	\return Animation&
*/
Animation & AnimationSet::GetAnimation(const string & name)
{
	Debug::CheckAssertion(HasAnimation(name), "Invalid animation name.");
	return mAnimations.find(name)->second;
}

/**
*	\brief 添加精灵
*
*	默认键值name唯一
*/
void AnimationSet::AddAnimation(const string & name, const Animation & animation)
{
	Debug::CheckAssertion(!HasAnimation(name), "Invalid animation name.");
	mAnimations[name] = animation;
}

/**
*	\brief 添加精灵
*
*	默认键值name唯一,并根据传入的方向数据，计算最大的碰撞盒
*/
void AnimationSet::AddAnimation(const string & name, const AnimationData & animationData)
{
	Debug::CheckAssertion(!HasAnimation(name), "Invalid animation name.");

	std::deque<AnimationDirection> animationDirections;
	// 创建direction对象
	for (const auto& direction : animationData.GetDirections())
	{
		Size size = direction.GetSize();
		mMaxSize.width = max(mMaxSize.width, size.width);
		mMaxSize.height = max(mMaxSize.height,size.height);
		mMaxBoundingBox |= direction.GetBoundingBox();

		animationDirections.emplace_back(direction.GetAllFrameRects(), direction.GetOrgin());
	}
	mAnimations.emplace(name, 
		Animation(animationData.GetImageName(),animationData.GetFrameDelay(),
				animationData.GetFrameLoop(),animationDirections));
}

Size AnimationSet::GetMaxSize() const
{
	return mMaxSize;
}

Rect AnimationSet::GetMaxBoundingBox() const
{
	return mMaxBoundingBox;
}

/**
*	\brief 从文件中初始化animationSet
*/
void AnimationSet::InitAnimationSetWithFile(const string & name)
{
	Debug::CheckAssertion(mAnimations.empty(), "Animation set already loaded.");

	string fileName = string("sprites") + name + ".dat";
	AnimationLuaData data;
	bool success = data.ImportFromFile(name);
	if (success)
	{
		mAnimationDefaultName = data.GetDefaultName();
		auto& animations = data.GetAnimations();
		for (auto& it : animations)
			AddAnimation(it.first, it.second);
	}

}
