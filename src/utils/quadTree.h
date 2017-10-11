#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include"common\common.h"
#include"utils\rectangle.h"
#include"game\sprite.h"
#include<set>
#include<array>

/**
*	\brief 四叉树结构
*/
template <typename T>
class QuadTree
{
public:
	QuadTree();

	void Initialize(const Rect& rect);
	bool Add(const T& element, const Rect& boundingBox);	   // 添加element
	bool Move(const T& element, const Rect& newBoundingBox);   // 移动element
	bool Remove(const T& element);
	void Clear();

	Rect GetSpace()const;
	std::vector<T> GetElements(const Rect& region)const;
	bool Contain(const T& element)const;

	void DrawDebug();
	SpritePtr mDebugSprite;

	static const int MaxElementsInCell = 8;	// 大于这个需要split
	static const int MinElementsInCell = 4;	// 小于这个需要merge
	static const int minCellWidth = 16;	
	static const int minCellHeight = 16;	

private:
	/**
	*	\brief 四叉树节点
	*/
	class Node
	{
	public:
		Node();
		Node(const Rect& box);

		void Initialize(const Rect& rect);
		bool Add(const T& element, const Rect& boundingBox);
		bool Remove(const T& element, const Rect& boundingBox);
		void GetElements(const Rect&region, std::set<T>& result)const;
		int GetElementCount()const;
		void Clear();

		Rect GetCell()const;
		bool Remove(const T& element);
		void DrawDebug(SpritePtr sprite);
	private:
		bool IsSplit()const;
		void Split();
		void Merge();

		Rect mCell;
		std::vector<std::pair<T, Rect>> mElements;
		std::array<std::unique_ptr<Node>, 4> mChilds;
		Color4B mDebugColor;
	};

	Node mRoot;								/** 四叉树的根节点 */
	std::map<T, Rect> mElements;			/** 保存所有添加的element */
	std::set<T> mElementsOut;				/** 保存四叉树范围外的element */
};

#include"utils\quadTree.inl"

#endif