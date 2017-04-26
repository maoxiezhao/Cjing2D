#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include"common\common.h"
#include"utils\rectangle.h"
#include<array>

template <typename T>
class QuadTree
{
public:
	QuadTree();

	bool Add(const T& element, const Rectangle& boundingBox);	   // 添加element
	bool Move();   // 移动element
	bool Remove();

	Rectangle GetSpace()const;
	std::vector<T> GetElements(const Rectangle& region)const;
	bool Contain(const T& element)const;

	static const int MaxElementsInCell = 8;	// 大于这个需要split
	static const int MinElementsInCell = 4;	// 小于这个需要merge

private:
	class Node
	{
	public:
		Node();

		void Add(const T& element, const Rectangle& boundingBox);
		void Remove(const T& element, const Rectangle& boundingBox);
		void GetElements(const Rectangle&region, std::set<T>& result);

		Rectangle GetCell()const;
		bool Remove(const T& element);
	
	private:
		bool IsSplit()const;
		void Split();
		void Merge();

		std::vector<std::pair<T, Rectangle>> mElements;
		std::array<std::unique_ptr<Node>, 4> mChilds;

	};

	Node mRoot;
	std::set<T> mElementsOut;
};

#include"quadTree.inl"

#endif