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
*	\brief ·�����������
*/
class PathFinding
{
public:
	virtual ~PathFinding() {};
	virtual std::string RequestPath(Entity& source, Entity& target, Map& map) = 0;
	virtual std::string RequestPath(Entity& source, const Point2& target, Map& map) = 0;
};

/**
*	\brief ����A*��Ѱ·�㷨
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

	/**	����Ѱ·���ڵ�,�ýڵ��¼λ�ú�����ֵF = G + H */
	struct Node
	{
		int index;		/** Ψһ��־Index */
		Point2 pos;		/** ��ǰλ�� */
		int Fcost;		/** �ܵ�Cost��F = G + H */
		int Gcost;		/** ����ýڵ���ۼ�Cost */
		int parentIndex;/** ���ڵ�Index */
		int dir;		/** �ƶ��ķ��� */
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