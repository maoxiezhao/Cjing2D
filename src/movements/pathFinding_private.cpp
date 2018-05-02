#include "pathFinding_private.h"
#include "entity\entity.h"
#include "core\debug.h"
#include "game\map.h"

namespace PathFindingPrivate {


namespace {
	const int FINDING_STEP_LINE_CONST = 8;
	const int FINDING_STEP_INCLINE_CONST = 11;
	const int FINDING_MAX_DISTANCE = 200;

	const int FINDING_STEP_LENGTH = 8;
	std::map<int, Point2> PosOffsets = {
		{ DIRECTION8_RIGHT,		{  1,  0 } },
		{ DIRECTION8_RIGHT_UP,	{  1, -1 } },
		{ DIRECTION8_UP,		{  0, -1 } },
		{ DIRECTION8_UP_LEFT,	{ -1, -1 } },
		{ DIRECTION8_LEFT,		{ -1,  0 } },
		{ DIRECTION8_LEFT_DOWN,	{ -1,  1 } },
		{ DIRECTION8_DOWN,		{  0,  1 } },
		{ DIRECTION8_DOWN_RIGHT,{  1,  1 } }
	};

	/** 获取曼哈顿距离 */
	int GetManhattanDistance(const Point2& p1, const Point2& p2)
	{
		return abs(p2.x - p1.x) + abs(p2.y - p1.y);
	}
}

AStartPathFinding::AStartPathFinding()
{
}

AStartPathFinding::~AStartPathFinding()
{
	Reset();
}

std::string AStartPathFinding::RequestPath(Entity & source, Entity & target, Map & map)
{
	std::string bestPath;
	// 如果目标entity是障碍物，则尝试从目标周围找到一个最近的路径
	if (target.IsObstacle(source))
	{
		Point2 targetPos = target.GetPos();
		Size size = target.GetSize();
		const std::vector<Point2> offsets = {
			{  size.width, 0 },
			{ -size.width, 0 },
			{ 0,  size.height },
			{ 0, -size.height }
		};

		size_t minSteps = std::numeric_limits<int>::max();
		for (const auto& offset : offsets)
		{
			std::string curPath = ComputePath(source, targetPos + offset, map);
			if (!curPath.empty() && curPath.size() < minSteps)
			{
				bestPath = curPath;
				minSteps = curPath.size();
			}
		}
	}
	else
	{	// 不然则直接计算目标entity的当前位置的路径
		bestPath = ComputePath(source, target.GetPos(), map);
	}
	return bestPath;
}

std::string AStartPathFinding::RequestPath(Entity & source, const Point2 & targetPos, Map & map)
{
	std::string bestPath;;

	// 获取目标点所有障碍物
	Rect rect = source.GetRectBounding();
	Point2 orgin = source.GetOrigin();
	rect.SetPos(targetPos.x - orgin.x,targetPos.y - orgin.y);

	auto obstacles = map.GetObstacles(rect, source);
	if (obstacles.empty())
	{
		bestPath = ComputePath(source, targetPos, map);
	}
	else
	{
		// 计算障碍物范围之外的安全目标点
		Rect obstacleRect = rect;
		for (const auto& obstacle : obstacles)
		{
			Rect r = obstacle->GetRectBounding();
			obstacleRect |= r;
		}
		Size size = obstacleRect.GetSize();
		const std::vector<Point2> offsets = {
			{ size.width, 0 },
			{ -size.width, 0 },
			{ 0,  size.height },
			{ 0, -size.height }
		};

		size_t minSteps = std::numeric_limits<int>::max();
		for (const auto& offset : offsets)
		{
			std::string curPath = ComputePath(source, targetPos + offset, map);
			if (!curPath.empty() && curPath.size() < minSteps)
			{
				bestPath = curPath;
				minSteps = curPath.size();
			}
		}
	}
	return bestPath;
}

std::string AStartPathFinding::ComputePath(Entity & source, const Point2 & target, Map & map)
{
	std::string curPath("");

	int targetIndex = GetNodeIndexByPos(target);
	const Point2 sourcePos = source.GetPos();
	int distance = GetManhattanDistance(sourcePos, target);
	if (distance > FINDING_MAX_DISTANCE)
	{
		std::ostringstream oss;
		oss << "The finding target pos(" << target.x << "," << target.y;
		oss << ") is too far from source target " << source.GetName();
		Debug::Warning(oss.str());
	}

	// 初始化source node
	Reset();
	Node node = CreateNode(sourcePos, distance);
	node.Fcost = distance;

	int index = node.index;
	mOpenList[index] = node;
	mOpenIndexList.push_back(index);

	bool finished = false;
	while (!finished && !mOpenIndexList.empty())
	{
		const auto curIndex = mOpenIndexList.front();
		Node node = mOpenList[curIndex];

		mOpenIndexList.pop_front();
		mOpenList.erase(curIndex);
		mClosedList[curIndex] = node;

		// 如果当前在目标范围内（8pixel内），则构建路径
		if (targetIndex == curIndex)
		{
			finished = true;
			curPath = BuildPathByNode(node);
		}
		else
		{
			auto IsCurNodeEnable = [&, target](Node& node, int dir)->bool {
				return IsNodeCanTransition(node, dir, map, source) &&
					GetManhattanDistance(node.pos, target) < FINDING_MAX_DISTANCE;
			};
			// 根据8方向创建节点
			for (int i = 0; i < 8; i++)
			{
				Point2 newPos = node.pos + PosOffsets[i] * FINDING_STEP_LENGTH;
				int heuristic = GetManhattanDistance(newPos, target);
				int stepCost = (i & 1) ? FINDING_STEP_INCLINE_CONST : FINDING_STEP_LINE_CONST;
				Node newNode = CreateNode(newPos, heuristic);
				newNode.Gcost = node.Gcost + stepCost;
				newNode.Fcost = newNode.Gcost + heuristic;

				int newIndex = newNode.index;
				const bool inClosedList = mClosedList.find(newIndex) != mClosedList.end();
				if (!inClosedList && IsCurNodeEnable(newNode, i))
				{
					if (mOpenList.find(newIndex) != mOpenList.end())
					{
						// 新的节点已经在OpenList中，则更新节点数据
						Node* existingNode = &mOpenList[newIndex];
						if (newNode.Fcost < existingNode->Fcost)
						{
							existingNode->Gcost = newNode.Gcost;
							existingNode->Fcost = newNode.Fcost;
							existingNode->parentIndex = newNode.parentIndex;
							// queue resort
							ResortNodeIndex(newNode);
						}
					}
					else
					{	// 添加新的节点
						newNode.dir = i;
						newNode.parentIndex = curIndex;
						mOpenList[newIndex] = newNode;
						// add node
						AddNodeIndex(newNode);;
					}
				}
				//else
				//{
				//	// 处理目标临时出现障碍物的情况,仅仅创建到达当前路径
				//	// 并立即返回，待下次请求时重新计算路径,这种判断必须
				//  // 保证entity对齐了网格
				//	if (newIndex == targetIndex)
				//	{
				//		finished = true;
				//		curPath = BuildPathByNode(node);
				//		break;
				//	}
				//}
			}
		}
	}
	return curPath;
}

void AStartPathFinding::Reset()
{	
	mOpenIndexList.clear();
	mOpenList.clear();
	mClosedList.clear();
}

std::string AStartPathFinding::BuildPathByNode(Node & node)
{
	std::string path("");
	const Node* curNode = &node;
	while (curNode->dir != DIRECTION8_NONE)
	{
		path = std::to_string(curNode->dir) + path;
		if (curNode->parentIndex == -1)
			break;
		
		curNode = &mClosedList[curNode->parentIndex];
	}
	return path;
}

/**
*	\brief 判断当前节点的指定方向移动是否有效
*/
bool AStartPathFinding::IsNodeCanTransition(Node & node, int dir, Map & map, Entity& source)
{
	// 第一次测试
	Rect rect = source.GetRectBounding();
	Point2 orgin = source.GetOrigin();
	rect.SetPos(node.pos.x - orgin.x,
		node.pos.y - orgin.y);

	if (map.TestCollisionWithObstacle(rect, source))
		return false;

	auto offset = PosOffsets[dir];
	// 第二次测试
	rect.AddPos(-offset.x * 7, -offset.y * 7);
	if (map.TestCollisionWithObstacle(rect, source))
		return false;

	return true;
}

AStartPathFinding::Node AStartPathFinding::CreateNode(const Point2 & pos, int heuristic)
{
	Node newNode;
	newNode.index = GetNodeIndexByPos(pos);
	newNode.pos = pos;
	newNode.Gcost = 0;
	newNode.Fcost = 0;
	newNode.dir = DIRECTION8_NONE;
	newNode.parentIndex = -1;

	return newNode;;
}

/**
*	\brief 根据位置计算当前index,保证基本单元为8pixel x 8pixel
*/
int AStartPathFinding::GetNodeIndexByPos(const Point2 & pos) const
{
	const int bottom = 10000;
	return bottom * (pos.y / 8) + (pos.x / 8);
}

/**
*	\brief 添加节点Index，同时保证最小的nodeIndex始终在列表首位
*/
void AStartPathFinding::AddNodeIndex(Node & node)
{
	bool inserted = false;
	for (auto it = mOpenIndexList.begin(); it != mOpenIndexList.end() && !inserted; ++it)
	{
		const Node* curNode = &mOpenList[(*it)];
		if (curNode->Fcost >= node.Fcost)
		{
			mOpenIndexList.insert(it, node.index);
			inserted = true;
		}
	}
	if (!inserted)
		mOpenIndexList.push_back(node.index);
}

/**
*	\brief 重新对indexList排序，当某个节点的F发生改变时调用
*/
void AStartPathFinding::ResortNodeIndex(Node& node)
{
	int curIndex = node.index;
	auto it = std::find(mOpenIndexList.begin(), mOpenIndexList.end(), curIndex);
	if (it != mOpenIndexList.end())
	{
		mOpenIndexList.erase(it);
		AddNodeIndex(node);
	}
}

}