#include"animationSprite.h"
#include"core\system.h"
#include"core\debug.h"

/**
*	\brief Ĭ�Ϲ��캯����Ŀǰ��Ҫ�����Ƿ��б�Ҫ�ṩ
*/
/*AnimationSprite::AnimationSprite()
{
}*/


AnimationSprite::AnimationSprite(const string & id):
	Sprite(),	// Ĭ�ϵĹ��캯��ֻ����programState��cameraMatrix
	mCurrAnimationSet(GetAnimationSet(id)),
	mFrameDelay(0),
	mFrameLoop(0),
	mFrameNum(0),
	mFrameFinished(false),
	mFrameChanged(false)
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
*	\brief ÿ֡���µ�ǰ֡����״̬
*/
void AnimationSprite::Update()
{
	Sprite::Update();

	if (IsSuspended())
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
*	\brief ���ƶ�������
*
*   ����ʱ��Ҫ�����Ƿ�����֡�任
*/
void AnimationSprite::Draw()
{
	if (IsFrameChanged())
		UpdateFramedChanged();

	Sprite::Draw();
}

/**
*	\brief �Ƿ��Ƕ������飬AnimationSprite��Ȼ����false;
*/
bool AnimationSprite::IsAnimationed() const
{
	return true;
}

/**
*	\brief �Ƿ���֡�任
*/
bool AnimationSprite::IsFrameChanged() const
{
	return mFrameChanged;
}

/**
*	\brief �Ƿ����õ�ǰ֡�����仯
*
*	���֡�����仯�����ڻ���ʱ�����updateFrameChaged()
*/
void AnimationSprite::SetFrameChanged(bool changed)
{
	mFrameChanged = changed;
}

/**
*	\brief ���õ�ǰ֡
*
*	�������õ�ǰ֡�����²��Ŷ���
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
*	\brief ���õ�ǰ����
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

		// �ȳ�ʼ��texture���ڸ���animation����rect
		SetCurrFrame(0);
		SetFrameChanged(true);
		
		InitWithTexture(mTexture);

		// luaContext���ܻ����һЩ����
		// ��onFrameChanged �� onDirectionChanged
		// ����
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

uint32_t AnimationSprite::GetFrameDelay() const
{
	return mFrameDelay;
}

/**
*	\brief ������һ֡
*
*	��������������򷵻�-1
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
*	\brief �����Ƿ���ͣ
*/
void AnimationSprite::SetSuspended(bool suspended)
{
	if (mSuspended != IsSuspended() )
	{
		Sprite::SetSuspended(suspended);
		if (!mSuspended)
		{
			// �����ͣ����Ҫ���¼�����һ֡�¼�
			uint32_t now = System::Now();
			mNextFrameDate = now + GetFrameDelay();
		}
	}
}

/**
*	\brief ���õ�ǰ����
*/
void AnimationSprite::ResetAnimation()
{
	SetCurrFrame(0);
}

/**
*	\brief ���¿�ʼ����
*/
void AnimationSprite::StartAnimation()
{
	ResetAnimation();
}

/**
*	\brief ֹͣ����
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
*	\brief ���ص�ǰ���������֡��
*/
int AnimationSprite::GetNumDirections() const
{
	if (mCurrAnimation == nullptr)
		return 0;
	return mCurrAnimation->GetNumDirections();
}

/**
*	\brief �����µĶ���������
*
*   ������õĵ�ǰ���Ʋ�ͬ���������붯��
*/
void AnimationSprite::SetCurrAnimationSetId(const string & id)
{
	if (id != GetCurrAnimationSetId())
	{
		mCurrAnimationSet = GetAnimationSet(id);
		SetCurrAnimation(mCurrAnimationSet.GetAnimationDefaultName());		// ��������������Ҫ���µ���֡
	}
}

/**
*	\brief ���ص�ǰ��������id����
*/
string AnimationSprite::GetCurrAnimationSetId() const
{
	return mCurrAnimationSet.GetAnimationSetName();
}

void AnimationSprite::NotifyFinished()
{
}

/**
*	\brief ��animation�����õ�ǰ�����Ϣ��ֻ����SetCurrAnimatio�е���
*/
void AnimationSprite::SetDataFromAnimation(const Animation & animation)
{
	mFrameDelay = animation.GetFrameDelay();
	mFrameLoop = animation.GetFrameLoop();
	mTexture = animation.GetTexture();

	//+++++++++++++++++++++++
}

/**
*	\brief ��ȡ��Ӧ��Animation
*
*	���������map�д�����ֱ�ӷ��أ�����������򴴽��󱣴���Map��
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
*	\brief ���±任��֡
*/
void AnimationSprite::UpdateFramedChanged()
{
	Rect rect = mCurrAnimation->GetAniamtionRect(mCurrFrame,mCurrDirection);
	SetTextureRect(rect, true);	
	SetFrameChanged(false);
}