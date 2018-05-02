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

	// ��Ϊ����Entity�õ���SharedPtr��entity�����ӳ�ɾ����
	// ����������Ҫ���Entity����
	if (mTargetEntity && mTargetEntity->IsBeRemoved())
		mTargetEntity = nullptr;

	if (IsSuspended())
		return;

	// ���Ѱ·�ƶ����,�������Ŀ��λ�û���Ŀ��entity�ֱ���
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
				{	// δ�ƶ���ָ��λ�ã����¼���·��
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
*	\brief ����Ѱ·Ŀ�꣬һ��Ѱ·�ڼ�ֻ��Ŀ��ĵ�ǰλ��Ѱ·
*/
void PathFindingMovement::SetTarget(EntityPtr targetEntity)
{
	if (targetEntity) {
		mTargetEntity = targetEntity;
		mNextRecomputeDate = System::Now() + 100;
	}
}

/**
*	\brief ����Ѱ·Ŀ��λ�ã������ǰ����pos
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
*	\brief ���㵱ǰ·��
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

		// �����ǰ����·����������һ����С��ʱ����
		// �н��Ѱ·������ͨ�����޽��С�ܶ�
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
*	\brief ����һ��������ƶ�·��
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
