#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include"common\common.h"
#include"utils\rectangle.h"
#include"game\sprite.h"
#include<set>
#include<array>

/**
*	\brief �Ĳ����ṹ
*/
template <typename T>
class QuadTree
{
public:
	QuadTree();

	void Initialize(const Rect& rect);
	bool Add(const T& element, const Rect& boundingBox);	   // ���element
	bool Move(const T& element, const Rect& newBoundingBox);   // �ƶ�element
	bool Remove(const T& element);
	void Clear();

	Rect GetSpace()const;
	std::vector<T> GetElements(const Rect& region)const;
	bool Contain(const T& element)const;

	void DrawDebug();
	SpritePtr mDebugSprite;

	static const int MaxElementsInCell = 8;	// ���������Ҫsplit
	static const int MinElementsInCell = 4;	// С�������Ҫmerge
	static const int minCellWidth = 16;	
	static const int minCellHeight = 16;	

private:
	/**
	*	\brief �Ĳ����ڵ�
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

	Node mRoot;								/** �Ĳ����ĸ��ڵ� */
	std::map<T, Rect> mElements;			/** ����������ӵ�element */
	std::set<T> mElementsOut;				/** �����Ĳ�����Χ���element */
};

#include"utils\quadTree.inl"

#endif