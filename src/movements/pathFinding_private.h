#pragma once

#include"utils\point.h"

#include<vector>
#include<list>
#include<string>
#include<map>

class Map;
class Entity;

namespace PathFindingPrivate {

/**
*	\brief 路径查找虚基类
*/
class PathFinding
{
public:
	virtual ~PathFinding() {};
	virtual std::string RequestPath(Entity& source, Entity& target, Map& map) = 0;
	virtual std::string RequestPath(Entity& source, const Point2& target, Map& map) = 0;
};

/**
*	\brief 基于A*的寻路算法
*/
class AStartPathFinding : public PathFinding
{
public:
	AStartPathFinding();
	virtual~AStartPathFinding();

	virtual std::string RequestPath(Entity& source, Entity& target, Map& map);
	virtual std::string RequestPath(Entity& source, const Point2& targetPos, Map& map);

private:
	std::string ComputePath(Entity& source, const Point2& target, Map& map);

	/**	构建寻路树节点,该节点记录位置和期望值F = G + H */
	struct Node
	{
		int index;		/** 唯一标志Index */
		Point2 pos;		/** 当前位置 */
		int Fcost;		/** 总的Cost，F = G + H */
		int Gcost;		/** 到达该节点的累计Cost */
		int parentIndex;/** 父节点Index */
		int dir;		/** 移动的方向 */
	};

	Node CreateNode(const Point2& pos, int heuristic);
	int GetNodeIndexByPos(const Point2& pos)const;
	void AddNodeIndex(Node& node);
	void ResortNodeIndex(Node& node);

	void Reset();
	std::string BuildPathByNode(Node& node);
	bool IsNodeCanTransition(Node& node, int dir, Map& map, Entity& source);
private:
	std::map<int, Node> mOpenList;
	std::map<int, Node> mClosedList;
	std::list<int> mOpenIndexList;
};
}