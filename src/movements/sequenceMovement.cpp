#include "sequenceMovement.h"
#include "core\debug.h"
#include "lua\luaContext.h"

SequenceMovement::SequenceMovement(bool isIngroedObstacles, bool looped):
	Movement(isIngroedObstacles),
	mCurrMovementIndex(0),
	mCurrMovement(nullptr),
	mFinished(false),
	mLooped(looped)
{
}

SequenceMovement::~SequenceMovement()
{
	mMovementSequence.clear();
}

/**
*	\brief 更新movement
*/
void SequenceMovement::Update()
{
	if (!IsSuspended() && !IsFinished())
	{
		if (mCurrMovement == nullptr || mCurrMovement->IsFinished())
		{
			mCurrMovement = GetNextMovement();
			if (mCurrMovement == nullptr)
			{
				SetFinished(true);
			}
			else
			{	// 启动当前movement
				mCurrMovement->Start();
			}
		}
		else
		{
			mCurrMovement->Update();
		}
	}
	Movement::Update();
}

bool SequenceMovement::IsSuspended() const
{
	if (mCurrMovement != nullptr)
	{
		return mCurrMovement->IsSuspended();
	}
	return Movement::IsSuspended();
}

void SequenceMovement::SetSuspended(bool suspended)
{
	if (mCurrMovement != nullptr)
	{
		mCurrMovement->SetSuspended(suspended);
	}
	Movement::SetSuspended(suspended);
}

/**
*	\brief 设置当前movment drawable宿主
*
*	务必在调用该函数前，已经添加所有的movmemnt
*/
void SequenceMovement::SetDrawable(Drawable * drawable)
{
	Movement::SetDrawable(drawable);
	for (auto& movement : mMovementSequence)
	{
		movement->SetDrawable(drawable);
	}
}

/**
*	\brief 设置当前movment entity宿主
*
*	务必在调用该函数前，已经添加所有的movmemnt
*/
void SequenceMovement::SetEntity(Entity * entity)
{
	Movement::SetEntity(entity);
	for (auto& movement : mMovementSequence)
	{
		movement->SetEntity(entity);
	}
}

void SequenceMovement::SetFinished(bool finished)
{
	if (mCurrMovement != nullptr)
	{
		mCurrMovement->Stop();
	}
	mFinished = finished;
}

/**
*	\brief 是否结束
*/
bool SequenceMovement::IsFinished() const
{
	return mFinished;
}

void SequenceMovement::Start()
{
	Debug::CheckAssertion(mCurrMovement == nullptr,
		"Have current movment before Sequence movememnt started.");
	mCurrMovement = GetNextMovement();
	Movement::Start();
}

void SequenceMovement::Stop()
{
	if (mCurrMovement != nullptr)
	{
		mCurrMovement->Stop();
	}
	Movement::Stop();
}

/**
*	\brief 查找是否存在某个movement
*/
bool SequenceMovement::HasMovement(const MovementPtr & movement)
{
	return std::find(mMovementSequence.begin(), mMovementSequence.end(), movement) != mMovementSequence.end();
}

/**
*	\brief 添加movement到动作序列中
*/
void SequenceMovement::AddMovement(MovementPtr movement)
{
	Debug::CheckAssertion(!IsStarted(),
		"Can not add movment when sequence movment is started.");

	if (HasMovement(movement))
	{
		Debug::Warning("Add the movement has already added.");
		return ;
	}
	mMovementSequence.push_back(movement);
}

/**
*	\brief 返回movement序列中下一个movement
*/
MovementPtr SequenceMovement::GetNextMovement()
{
	if (mMovementSequence.empty())
	{
		return nullptr;
	}
	// 如果循环则返回第一个movement
	if (mMovementSequence.size() <= mCurrMovementIndex)
	{
		if (mLooped )
		{
			mCurrMovementIndex = 0;
			return mMovementSequence[mCurrMovementIndex];
		}
		return nullptr;
	}
	return mMovementSequence[mCurrMovementIndex++];
}

const string SequenceMovement::GetLuaObjectName() const
{
	return string();
}
