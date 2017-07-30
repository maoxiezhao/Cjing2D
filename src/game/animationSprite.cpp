#include"animationSprite.h"
#include"core\system.h"
#include"core\debug.h"
#include"lua\luaContext.h"

/**
*	\brief 默认构造函数，目前需要考虑是否有必要提供
*/
/*AnimationSprite::AnimationSprite()
{
}*/


AnimationSprite::AnimationSprite(const string & id) :
	Sprite(),	// 默认的构造函数只创建programState和cameraMatrix
	mCurrAnimationSet(GetAnimationSet(id)),
	mFrameDelay(0),
	mFrameLoop(0),
	mFrameNum(0),
	mFrameFinished(false),
	mFrameChanged(false),
	mPaused(true)
{
	SetCurrAnimation(mCurrAnimationSet.GetAnimationDefaultName());
}

AnimationSprite::~AnimationSprite()
{
	for (auto& itr : mAllAnimationSets)
		delete itr.second;
	mAllAnimationSets.clear();
}

/**
*	\brief 每帧更新当前帧动画状态
*/
void AnimationSprite::Update()
{
	Sprite::Update();

	if (IsSuspended())
		return;

	if (mPaused)
		return;

	uint32_t now = System::Now();
	while (!mFrameFinished && !IsSuspended() && 
		GetFrameDelay() > 0 && now >= mNextFrameDate)
	{
		int nextFrame = GetNextFrame();

		if (nextFrame == -1)
		{
			mFrameFinished = true;
			NotifyFinished();
		}
		else
		{
			mCurrFrame = nextFrame;
			mNextFrameDate += GetFrameDelay();
		}
		SetFrameChanged(true);

		// luaContext do onFrameChanged
	}
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
*	\brief 是否设置当前帧发生变化
*
*	如果帧发生变化，则在绘制时会调用updateFrameChaged()
*/
void AnimationSprite::SetFrameChanged(bool changed)
{
	mFrameChanged = changed;
}

/**
*	\brief 设置当前帧
*
*	重新设置当前帧会重新播放动画
*/
void AnimationSprite::SetCurrFrame(int currFrame)
{
	Debug::CheckAssertion(currFrame >= -1 && currFrame < mCurrAnimation->GetNumFrame(mCurrDirection),
		"Invaild currFrame in AnimationSprite::setCurrFrame().");
	
	mFrameFinished = false;
	mNextFrameDate = System::Now() + GetFrameDelay();

	if (currFrame != mCurrFrame)
	{
		mCurrFrame = currFrame;
		SetFrameChanged(true);
	}
}


int AnimationSprite::GetCurrFrame() const
{
	return mCurrFrame;
}

void AnimationSprite::SetFrameFinished(bool finished)
{
	mFrameFinished = finished;
}

/**
*	\brief 设置当前动画
*/
void AnimationSprite::SetCurrAnimation(const string & name)
{
	if (name != mCurrAnimationName || !IsFrameStarted())
	{
		mCurrAnimationName = name;
		if (mCurrAnimationSet.HasAnimation(name))
		{
			mCurrAnimation = &mCurrAnimationSet.GetAnimation(name);
			SetDataFromAnimation(*mCurrAnimation);
		}
		else
			mCurrAnimation = nullptr;

		if (mCurrDirection < 0 || mCurrDirection >= GetNumDirections())
			mCurrDirection = 0;

		// 先初始化texture，在根据animation设置rect
		SetCurrFrame(0);
		SetFrameChanged(true);
		
		InitWithTexture(mTexture);

		// luaContext可能会存在一些操作
		// 如onFrameChanged 或 onDirectionChanged
		// 待加
	}
}

bool AnimationSprite::IsFrameFinished() const
{
	return mFrameFinished;
}

bool AnimationSprite::IsFrameStarted() const
{
	return !IsFrameFinished();
}

void AnimationSprite::SetFrameDelay(uint32_t delay)
{
	mFrameDelay = delay;
}

void AnimationSprite::SetPaused(bool paused)
{
	mPaused = paused;
}

uint32_t AnimationSprite::GetFrameDelay() const
{
	return mFrameDelay;
}

/**
*	\brief 返回下一帧
*
*	如果动画结束，则返回-1
*/
int AnimationSprite::GetNextFrame() const
{
	if (mCurrAnimation == nullptr)
		return -1;
	return mCurrAnimation->GetNextFrame(mCurrFrame,mCurrDirection);
}

int AnimationSprite::GetNumFrames() const
{
	if (mCurrAnimation == nullptr)
		return 0;
	return mCurrAnimation->GetDirection(mCurrDirection).GetNumFrames();
}

/**
*	\brief 设置是否暂停
*/
void AnimationSprite::SetSuspended(bool suspended)
{
	if (suspended != IsSuspended() )
	{
		Sprite::SetSuspended(suspended);
		if (!suspended)
		{
			// 解除暂停后，需要重新计算下一帧事件
			uint32_t now = System::Now();
			mNextFrameDate = now + GetFrameDelay();
		}
	}
}

/**
*	\brief 重置当前动画
*/
void AnimationSprite::ResetAnimation()
{
	SetPaused(false);
	SetCurrFrame(0);
}

/**
*	\brief 重新开始动画
*/
void AnimationSprite::StartAnimation()
{
	ResetAnimation();
}

/**
*	\brief 停止动画
*/
void AnimationSprite::StopAnimation()
{
	mFrameFinished = true;
}

int AnimationSprite::GetCurrDirection() const
{
	return mCurrDirection;
}

/**
*	\brief 返回当前动画的最大帧数
*/
int AnimationSprite::GetNumDirections() const
{
	if (mCurrAnimation == nullptr)
		return 0;
	return mCurrAnimation->GetNumDirections();
}

/**
*	\brief 设置当前的方向
*/
void AnimationSprite::SetCurrDirection(int direction)
{
	Debug::CheckAssertion(direction >= -1 && direction < mCurrAnimation->GetNumDirections(),
		"Invaild direction in AnimationSprite::SetCurrDirection().");
	mCurrDirection = direction;
}

/**
*	\brief 设置新的动画集名称
*
*   如果设置的当前名称不同会重新载入动画
*/
void AnimationSprite::SetCurrAnimationSetId(const string & id)
{
	if (id != GetCurrAnimationSetId())
	{
		mCurrAnimationSet = GetAnimationSet(id);
		SetCurrAnimation(mCurrAnimationSet.GetAnimationDefaultName());		// 更换动画集后需要重新调整帧
	}
}

/**
*	\brief 返回当前动画集的id名称
*/
string AnimationSprite::GetCurrAnimationSetId() const
{
	return mCurrAnimationSet.GetAnimationSetName();
}

void AnimationSprite::NotifyFinished()
{
}

const string AnimationSprite::GetLuaObjectName() const
{
	return LuaContext::module_animation_name;
}

/**
*	\brief 从animation中设置当前相关信息，只能在SetCurrAnimatio中调用
*/
void AnimationSprite::SetDataFromAnimation(const Animation & animation)
{
	mFrameDelay = animation.GetFrameDelay();
	mFrameLoop = animation.GetFrameLoop();
	mTexture = animation.GetTexture();

	//+++++++++++++++++++++++
}

/**
*	\brief 获取对应的Animation
*
*	如果对象在map中存在则直接返回，如果不存在则创建后保存在Map中
*/
AnimationSet & AnimationSprite::GetAnimationSet(const string & id)
{
	AnimationSet* animationSet = nullptr;
	auto it = mAllAnimationSets.find(id);
	if (it != mAllAnimationSets.end())
		animationSet = it->second;
	else
	{
		animationSet = new AnimationSet(id);
		mAllAnimationSets[id] = animationSet;
	}
	return *animationSet;
}

/**
*	\brief 更新变换的帧
*/
void AnimationSprite::UpdateFramedChanged()
{
	Rect rect = mCurrAnimation->GetAniamtionRect(mCurrFrame,mCurrDirection);
	SetTextureRect(rect, true);	
	SetFrameChanged(false);
}
