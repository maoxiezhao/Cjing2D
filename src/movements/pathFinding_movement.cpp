#include "pathFinding_movement.h"
#include "core\system.h"
#include "core\random.h"
#include "utils\geometry.h"
#include "lua\luaContext.h"

const int SHIFT_EPSILON = 8;

PathFindingMovement::PathFindingMovement(int speed, bool ingoreObstacle):
	PathMovement("", speed, false, ingoreObstacle),
	mTargetEntity(nullptr),
	mTargetPoint(-1, -1),
	mNextRecomputeDate(0),
	mRandomPath(false),
	mPathFinding(std::make_unique<
		PathFindingPrivate::AStartPathFinding>())
{
}

void PathFindingMovement::Update()
{
	PathMovement::Update();

	// 因为这里Entity用的是SharedPtr，entity会做延迟删除，
	// 所以在这里要清除Entity引用
	if (mTargetEntity && mTargetEntity->IsBeRemoved())
		mTargetEntity = nullptr;

	if (IsSuspended())
		return;

	// 如果寻路移动完成,则根据是目标位置还是目标entity分别处理
	if (PathMovement::IsFinished())
	{
		uint32_t now = System::Now();
		if (now >= mNextRecomputeDate)
		{
			if (mTargetEntity)
			{
				RecomputePath();
			}
			else if (mTargetPoint != Point2(-1, -1))
			{
				Point2 curPos = GetPos();
				if (Geometry::GetDistance(curPos, mTargetPoint) <= SHIFT_EPSILON)
				{
  					mTargetPoint = { -1, -1 };
					NotifyPathFindingFinished();
				}
				else
				{	// 未移动到指定位置，重新计算路径
					RecomputePath();
				}
			}
		}
	}
}

bool PathFindingMovement::IsFinished() const
{
	return false;
}

/**
*	\brief 设置寻路目标，一次寻路期间只对目标的当前位置寻路
*/
void PathFindingMovement::SetTarget(EntityPtr targetEntity)
{
	if (targetEntity) {
		mTargetEntity = targetEntity;
		mNextRecomputeDate = System::Now() + 100;
	}
}

/**
*	\brief 设置寻路目标位置，如果当前存在pos
*/
void PathFindingMovement::SetTarget(const Point2 & targetPos, bool clearEntity)
{
	if (clearEntity)
		SetTarget(nullptr);

	mTargetPoint = targetPos;

	if (mTargetEntity == nullptr)
		mNextRecomputeDate = System::Now() + 100;
}

void PathFindingMovement::NotifyPathFindingFinished()
{
	auto curEntity = GetEntity();
	if (curEntity)
		curEntity->NotifyPathFindingFinished();
}

const string PathFindingMovement::GetLuaObjectName() const
{
	return LuaContext::module_path_finding_movement_name;
}

/**
*	\brief 计算当前路径
*/
void PathFindingMovement::RecomputePath()
{
	std::string path("");
	uint32_t minDelay = 0;
	if (mPathFinding)
	{
		auto& curEntity = *GetEntity();
		Map& map = curEntity.GetMap();
		if (mTargetEntity)
		{
			path = mPathFinding->RequestPath(curEntity, *mTargetEntity, map);
		}
		else if (mTargetPoint != Point2(-1, -1))
		{
			path = mPathFinding->RequestPath(curEntity, mTargetPoint, map);
		}

		// 如果当前存在路径，则设置一个较小的时间间隔
		// 有解的寻路的消耗通常比无解的小很多
		if (path.size() == 0)
		{	
			minDelay = 3000;
			if (mRandomPath)
				path = CreateRandomPath();
		}
		else
		{
			minDelay = 300;
		}
	}
	mNextRecomputeDate = System::Now() + minDelay + Random::GetRandomInt(200);
	SetPathString(path);
}

/**
*	\brief 创建一个随机的移动路径
*/
std::string PathFindingMovement::CreateRandomPath()
{
	std::string path = "";
	char dir = '0' + Random::GetRandomInt(8);
	int len = Random::GetRandomInt(5) + 3;
	for (int i = 0; i < len; i++)
		path += dir;

	return path;
}
