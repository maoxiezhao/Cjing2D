#include"utils\rectangle.h"
#include "quadTree.h"

template <typename T>
QuadTree<T>::QuadTree():
	mDebugSprite(nullptr)
{
	Initialize({ 0, 0, 640, 480 });
}

template<typename T>
inline void QuadTree<T>::Initialize(const Rect & rect)
{
	Clear();

	mRoot.Initialize(rect);
}

/**
*	\brief 添加element
*	\param element 添加的节点
*	\param boundingBox 该节点的rect
*	\return true添加成功
*/
template<typename T>
inline bool QuadTree<T>::Add(const T & element, const Rect & boundingBox)
{
	if (Contain(element))
		return false;
	if (!boundingBox.Overlaps(GetSpace()))
	{
		mElementsOut.insert(element);
		return true;
	}
	if (!mRoot.Add(element, boundingBox))
		return false;

	mElements[element] = boundingBox;
	return true;
}

/**
*	\brief 调整element在quadTree中位置
*
*	当element的pos和size发生改变时，调用该函数来保持四叉树结构的
*	的正确性。
*/
template<typename T>
inline bool QuadTree<T>::Move(const T & element, const Rect & newBoundingBox)
{
	auto& it = mElements.find(element);
	if (it == mElements.end())
	{	// 四叉树中未保存当前结构
		return false;
	}
	else
	{
		// 当rect未发生改变时直接返回
		if (it->second == newBoundingBox)
		{
			return true;
		}
		// 先移除old element，再添加新的element
		if (!Remove(element))
		{
			return false;
		}
		if (!Add(element, newBoundingBox))
		{
			return false;
		}
	}
	return true;
}

/**
*	\brief 移除指定的element
*/
template<typename T>
inline bool QuadTree<T>::Remove(const T& element)
{
	auto it = mElements.find(element);
	if (it != mElements.end())
	{
		const Rect rect = it->second;
		mElements.erase(element);
		if (mElementsOut.erase(element) > 0)
		{
			return true;
		}
		// 如果element在四叉树范围内，则从根节点开始往下遍历移除
		return mRoot.Remove(element, rect);
	}

	return false;
}

template<typename T>
inline void QuadTree<T>::Clear()
{
	mElements.clear();
	mElementsOut.clear();
	mRoot.Clear();
}

template<typename T>
inline Rect QuadTree<T>::GetSpace() const
{
	return mRoot.GetCell();
}

/**
*	\brief 获取当前region范围内element
*	\return 返回不包含重复element的vector
*/
template<typename T>
inline std::vector<T> QuadTree<T>::GetElements(const Rect & region) const
{
	std::vector<T> result;
	std::set<T> sets;
	mRoot.GetElements(region, sets);

	for (auto& element : sets)
	{
		result.push_back(element);
	}
	return result;
}

template<typename T>
inline bool QuadTree<T>::Contain(const T & element) const
{
	return mElements.find(element) != mElements.end();
}

/**
*	\brief Debug绘制
*/
template<typename T>
inline void QuadTree<T>::DrawDebug()
{
	if (mDebugSprite == nullptr)
	{
		mDebugSprite = std::make_shared<Sprite>(Color4B(rand() % (255), rand() % (255), rand() % (255), 255), Size(0, 0));
	}
	// draw node
	mRoot.DrawDebug(mDebugSprite);
}

/****************************************************************************
*						     QuadTree Node
****************************************************************************/

template<typename T>
QuadTree<T>::Node::Node()
{
	mDebugColor = Color4B(rand() % (255), rand() % (255), rand() % (255), 255);
}

template<typename T>
inline QuadTree<T>::Node::Node(const Rect & box) :
	mCell(box)
{
	mDebugColor = Color4B(rand() % (255), rand() % (255), rand() % (255), 255);
}

/**
*	\brief 初始化根节点
*/
template<typename T>
inline void QuadTree<T>::Node::Initialize(const Rect & rect)
{
	mElements.clear();
	mElements.reserve(MaxElementsInCell);
	mCell = rect;
}

/**
*	\brief 添加新的element
*/
template<typename T>
inline bool QuadTree<T>::Node::Add(const T & element, const Rect & boundingBox)
{
	// 如果不在该节点的范围内
	if (!GetCell().Overlaps(boundingBox))
	{
		return false;
	}

	// 如果未划分则判断是否需要划分
	// 否则直接加入到elements中
	if (!IsSplit())
	{
		if (mElements.size() >= MaxElementsInCell &&
			GetCell().width > minCellWidth &&
			GetCell().height > minCellHeight)
		{
			Split();
		}
	}
	if (!IsSplit())
	{
		mElements.emplace_back(element, boundingBox);
		return true;
	}

	// 如果已经划分，则直接添加到各个划分节点中
	for (auto& child : mChilds)
	{
		child->Add(element, boundingBox);
	}
	return true;
}

/**
*	\brief 从当前节点中删除指定的element
*	\param boundingBox 该element的rect范围
*/
template<typename T>
inline bool QuadTree<T>::Node::Remove(const T & element, const Rect & boundingBox)
{
	if (!GetCell().Overlaps(boundingBox))
	{
		return false;
	}

	if (!IsSplit())
	{
		// 未划分则直接从elements中删除
		const auto& it = std::find(mElements.begin(), mElements.end(),
				std::make_pair(element, boundingBox));
		if (it != mElements.end())
		{
			mElements.erase(it);
			return true;
		}
		return false;
	}
	else
	{	// 如果已经划分，则各个子节点继续删除动作
		bool removed = false;
		for (auto& child : mChilds)
		{
			removed |= child->Remove(element, boundingBox);
		}
		// 当存在删除之后，且该节点子节点为叶子节点，考虑是否需要合并
		if (removed && !mChilds[0]->IsSplit())
		{
			int elementCount = GetElementCount();
			if (elementCount < MinElementsInCell)
			{
				Merge();
			}
		}
		return removed;
	}
}

/**
*	\brief 获取当前节点下范围内的element
*	\param region 范围的rect
*	\param result 存放获取的element
*/
template<typename T>
inline void QuadTree<T>::Node::GetElements(const Rect & region, std::set<T>& result)const
{
	if (!GetCell().Overlaps(region))
	{
		return;
	}

	// 如果已经划分则从4个子节点中获取
	if (IsSplit())
	{
		for (auto& child : mChilds)
		{
			child->GetElements(region, result);
		}
	}
	else
	{	// 遍历当前节点保存的element，并判断是否满足获取条件
		for (auto& element : mElements)
		{
			if (element.second.Overlaps(region) &&
				result.find(element.first) == result.end())
			{
				result.insert(element.first);
			}
		}
	}
}

/**
*	\brief 获取当前节点的element数量
*/
template<typename T>
inline int QuadTree<T>::Node::GetElementCount() const
{
	int result = 0;
	if (IsSplit())
	{
		for (auto& child : mChilds)
		{
			result += child->GetElementCount();
		}
	}
	else
	{
		result = mElements.size();
	}
	return result;
}

template<typename T>
inline void QuadTree<T>::Node::Clear()
{
	mElements.clear();
	if (IsSplit())
	{
		for (auto& child : mChilds)
		{
			child->Clear();
		}
	}
	std::fill(mChilds.begin(), mChilds.end(), nullptr);
}

template<typename T>
inline Rect QuadTree<T>::Node::GetCell() const
{
	return mCell;
}

template<typename T>
inline bool QuadTree<T>::Node::Remove(const T & element)
{
	
	return false;
}

/**
*	\brief 绘制当前quadTree的划分状态和所包含的element的rect
*/
template<typename T>
inline void QuadTree<T>::Node::DrawDebug(SpritePtr sprite)
{
	const Rect rect = GetCell();
	sprite->SetColor(mDebugColor);
	sprite->SetPos(rect.GetPos());
	sprite->SetSize(rect.GetSize());
	sprite->MultiplyDraw();

	if (!IsSplit())
	{
		for (auto& element : mElements)
		{
			Rect elementRect = element.second;
			sprite->SetColor(Color4B::WHITE);
			sprite->SetPos(elementRect.GetPos());
			sprite->SetSize(elementRect.GetSize());
			sprite->MultiplyDraw();
		}
	}
	else
	{
		for (auto& child : mChilds)
		{
			child->DrawDebug(sprite);
		}
	}
}

/**
*	\brief 该节点是否已经划分
*
*	可以保证4个划分必然全部同时划分或者释放
*/
template<typename T>
inline bool QuadTree<T>::Node::IsSplit() const
{
	return mChilds[0] != nullptr;
}

/**
*	\brief 划分该节点
*
*	可以保证4个划分必然全部同时划分或者释放
*/
template<typename T>
inline void QuadTree<T>::Node::Split()
{
	Debug::CheckAssertion(!IsSplit(),
		"The Quad Tree Node has already split.");

	Rect cellRect = GetCell();
	Size cellSizeInv2 = cellRect.GetSize() / 2;
	Point2 ltPos = cellRect.GetPos();
	Point2 centerPos = cellRect.GetCenterPos();

	// 创建划分节点
	mChilds[0] = std::unique_ptr<Node>(new Node( Rect(ltPos, cellSizeInv2)));
	mChilds[1] = std::unique_ptr<Node>(new Node( Rect(Point2(centerPos.x, ltPos.y), cellSizeInv2)));
	mChilds[2] = std::unique_ptr<Node>(new Node( Rect(Point2(ltPos.x, centerPos.y), cellSizeInv2)));
	mChilds[3] = std::unique_ptr<Node>(new Node( Rect(centerPos, cellSizeInv2)));

	// 将当前节点的element保存到划分节点
	for (auto& element : mElements)
	{
		for (auto& child : mChilds)
		{
			child->Add(element.first, element.second);
		}
	}
	mElements.clear();
}

/**
*	\brief 将已经划分的结点合并为叶节点
*
*	当执行remove操作是会调用该函数
*/
template<typename T>
inline void QuadTree<T>::Node::Merge()
{
	Debug::CheckAssertion(IsSplit(),
		"The Quad Tree Node has not split.");

	std::set<T> mMergedElement;
	for (auto& child : mChilds)
	{
		auto& elementPairs = child->mElements;
		for (auto& elementPair : elementPairs)
		{
			if (mMergedElement.insert(elementPair.first).second)
			{
				mElements.push_back(elementPair);
			}
		}
	}
	
	std::fill(mChilds.begin(), mChilds.end(), nullptr);
}
