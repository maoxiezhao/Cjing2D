#include"animationSprite.h"
#include"core\system.h"
#include"core\debug.h"
#include"lua\luaContext.h"

/**
*	\brief Ĭ�Ϲ��캯����Ŀǰ��Ҫ�����Ƿ��б�Ҫ�ṩ
*/
/*AnimationSprite::AnimationSprite()
{
}*/


AnimationSprite::AnimationSprite(const string & id) :
	Sprite(),	// Ĭ�ϵĹ��캯��ֻ����programState��cameraMatrix
	mCurrAnimationSet(GetAnimationSet(id)),
	mSize(),
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
*	\brief ÿ֡���µ�ǰ֡����״̬
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
*	\brief ���ƶ�������
*	\param ���Ƶ�λ��
*
*   ����ʱ��Ҫ�����Ƿ�����֡�任
*/
void AnimationSprite::Draw(const Point2 & pos)
{
	if (IsFrameChanged())
		UpdateFramedChanged();

	Sprite::Draw(pos);
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
	}
	SetFrameChanged(true);
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
		{
			mCurrAnimation = nullptr;
			Debug::Error("The animation:" + name + "is not exists.");
			return;
		}

		if (mCurrDirection < 0 || mCurrDirection >= GetNumDirections())
			mCurrDirection = 0;

		// �ȳ�ʼ��texture���ڸ���animation����rect
		SetCurrFrame(0);
		SetFrameChanged(true);
		
		InitWithTexture(mTexture);
		Rect rect = mCurrAnimation->GetAniamtionRect(mCurrFrame, mCurrDirection);
		mSize = Size(rect.width, rect.height);

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

void AnimationSprite::SetPaused(bool paused)
{
	mPaused = paused;
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

void AnimationSprite::SetSize(const Size & size)
{
	mSize = size;
}

Size AnimationSprite::GetSize() const
{
	return mSize;
}

/**
*	\brief �����Ƿ���ͣ
*/
void AnimationSprite::SetSuspended(bool suspended)
{
	if (suspended != IsSuspended() )
	{
		Sprite::SetSuspended(suspended);
		if (!suspended)
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
	SetPaused(false);
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
*	\brief ���õ�ǰ�ķ���
*/
void AnimationSprite::SetCurrDirection(int direction)
{
	Debug::CheckAssertion(direction >= -1 && direction < mCurrAnimation->GetNumDirections(),
		"Invaild direction in AnimationSprite::SetCurrDirection().");

	mCurrDirection = direction;
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

/**
*	\brief �����Ƿ����ָ������
*/
bool AnimationSprite::HasAnimation(const string & name) const
{
	return mCurrAnimationSet.HasAnimation(name);
}

void AnimationSprite::NotifyFinished()
{
}

const string AnimationSprite::GetLuaObjectName() const
{
	return LuaContext::module_animation_name;
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
	Sprite::SetSize(mSize);
	SetFrameChanged(false);
}
