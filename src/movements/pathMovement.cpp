#include "pathMovement.h"
#include "core\system.h"
#include "game\gameCommands.h"
#include "entity\entity.h"
#include "lua\luaContext.h"

std::map<int, Point2> PathMovement::pathDirectionShift = {
	{ DIRECTION8_RIGHT,	     {  1,  0 } },
	{ DIRECTION8_RIGHT_UP,   {  1, -1 } },
	{ DIRECTION8_UP,		 {  0, -1 } },
	{ DIRECTION8_UP_LEFT,    { -1, -1 } },
	{ DIRECTION8_LEFT,       { -1,  0 } },
	{ DIRECTION8_LEFT_DOWN,  { -1,  1 } },
	{ DIRECTION8_DOWN,       {  0,  1 } },
	{ DIRECTION8_DOWN_RIGHT, {  1,  1 } }
};

PathMovement::PathMovement(const std::string & path, int speed, bool loop, bool ingoreObstacles)
	:Movement(ingoreObstacles),
	mSpeed(speed),
	mLoop(loop),
	mCurPathDir(DIRECTION8_NONE),
	mCurSubStep(0),
	mSubStepFinished(false),
	mSubStepDelay(0),
	mStopedByObstacle(false),
	mFinished(true)
{
	SetPathString(path);
}

void PathMovement::Update()
{
	while (	HasOwner() &&
		!IsSuspended() && 
		IsCurPathStepFinished() &&
		!PathMovement::IsFinished())
	{
		StartNextPathStep();
		UpdateCurPathStep();
	}

	UpdateCurPathStep();
	Movement::Update();
}

bool PathMovement::IsStarted() const
{
	return !IsFinished();
}

void PathMovement::SetSuspended(bool suspended)
{
	Movement::SetSuspended(suspended);
	if (!suspended)
	{
		if (GetWhenSuspeneded() != 0 )
		{
			uint32_t diff = System::Now() - GetWhenSuspeneded();
			mNextStepDate += diff;
		}
	}
}

bool PathMovement::IsFinished() const
{
	return (mFinished && IsCurPathStepFinished() &&
		mCurPaths.empty() && !mLoop) || mStopedByObstacle;
}

void PathMovement::Start()
{
	Movement::Start();
	Restart();
}

void PathMovement::Stop()
{
	mFinished = true;
	mSubStepFinished = true;
}

/**
*	\brief ��ȡ��ǰ�ķ���8����Ҫ��ת����
*/
int PathMovement::GetDirection() const
{
	return mCurPathDir / 2;
}

void PathMovement::SetSpeed(int speed)
{
	mSpeed = speed;
	if (speed == 0)
		Stop();
}

int PathMovement::GetSpeed() const
{
	return mSpeed;
}

void PathMovement::SetLoop(bool loop)
{
	mLoop = loop;
	if (loop && IsFinished() && !mSetPaths.empty())
		Restart();
}

bool PathMovement::IsLoop() const
{
	return mLoop;
}

void PathMovement::SetPath(const std::list<int>& paths)
{
	mSetPaths.clear();
	mCurPaths.clear();

	for (const auto& path : paths)
	{
		mSetPaths.push_back(path);
		mCurPaths.push_back(path);
	}
	Restart();
}

void PathMovement::SetPathString(const std::string & pathsString)
{
	std::list<int> paths;
	for (const auto& path : pathsString)
	{
		int dir = path - '0';
		paths.push_back(dir);
	}

	if(!paths.empty())
		SetPath(paths);
}

bool PathMovement::IsCurPathStepFinished() const
{
	return mSubStepFinished;
}

const string PathMovement::GetLuaObjectName() const
{
	return LuaContext::module_path_movement_name;
}

void PathMovement::StartNextPathStep()
{
	//Entity* entity = GetEntity();
	//if (!entity)
	//	return;
	if (!HasOwner())
		return;

	// ��ǰ·��Ϊ�գ������������ˣ��������õĲ���Ϊ��
	if (mCurPaths.empty())
	{
		if (!mSetPaths.empty())	{
			if (mLoop) 
			{
				for (const auto& path : mSetPaths)
					mCurPaths.push_back(path);
			}
			else if (!mFinished)
			{
				mFinished = true;
			}
		}
		else
		{
			Stop();
		}
	}
	// ���·����Ϊ�գ������õ�ǰ·�����ٶ�
	if (!mCurPaths.empty())
	{
		mCurPathDir = mCurPaths.front();
		mCurPaths.pop_front();
		mCurSubStep = 0;
		mSubStepDelay = GetDelayBySpeed(mSpeed, mCurPathDir);
		mSubStepFinished = false;

		if (mLastPathDir != mCurPathDir)
		{
			NotifyMovementChanged();
			mLastPathDir = mCurPathDir;
		}

		SetNextStepDelay(mSubStepDelay);
	}
}

/**
*	\brief ���µ�ǰstep��ֱ�����8pixelλ�ƣ�8���ƶ���
*/
void PathMovement::UpdateCurPathStep()
{
	uint32_t now = System::Now();
	while (now >= mNextStepDate &&
		!mSubStepFinished &&
		!IsSuspended() &&
		((GetEntity() != nullptr && GetEntity()->GetMovement().get() == this) || HasOwner()))
	{
		auto oldPos = GetPos();
		// try move
		const auto& offset = pathDirectionShift[mCurPathDir];
		if (!TestCollisionWithObstacles(offset))
		{
			TranslatePos(offset);
		}
		mCurSubStep++;

		if (mCurSubStep >= 8)
			mSubStepFinished = true;

		if (!mSubStepFinished)
			mNextStepDate += mSubStepDelay;

		// ��������ײ
		if (oldPos == GetPos())
		{
			NotifyObstacleReached();
			mStopedByObstacle = true;
		}
	}
}

void PathMovement::SetNextStepDelay(uint32_t delay)
{
	uint32_t now = System::Now();
	mNextStepDate = now + delay;
}

/**
*	\brief ���ݵ�ǰ�ٶȺͷ����ȡ���¼��
*/
uint32_t PathMovement::GetDelayBySpeed(int speed, int dir) const
{
	uint32_t delay = 1000 / speed;
	if (dir % 2 != 0)  // б����
		delay = (uint32_t)(delay * Geometry::SQRT_2);

	return delay;
}

/**
*	\brief ��������PathMovement
*/
void PathMovement::Restart()
{
	mCurPaths.clear();
	for (const auto& path : mSetPaths)
		mCurPaths.push_back(path);

	mFinished = false;
	mStopedByObstacle = false;
	mLastPathDir = Direction8::DIRECTION8_NONE;
	StartNextPathStep();
}
