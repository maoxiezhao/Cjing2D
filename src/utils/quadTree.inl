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
*	\brief ���element
*	\param element ��ӵĽڵ�
*	\param boundingBox �ýڵ��rect
*	\return true��ӳɹ�
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
*	\brief ����element��quadTree��λ��
*
*	��element��pos��size�����ı�ʱ�����øú����������Ĳ����ṹ��
*	����ȷ�ԡ�
*/
template<typename T>
inline bool QuadTree<T>::Move(const T & element, const Rect & newBoundingBox)
{
	auto& it = mElements.find(element);
	if (it == mElements.end())
	{	// �Ĳ�����δ���浱ǰ�ṹ
		return false;
	}
	else
	{
		// ��rectδ�����ı�ʱֱ�ӷ���
		if (it->second == newBoundingBox)
		{
			return true;
		}
		// ���Ƴ�old element��������µ�element
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
*	\brief �Ƴ�ָ����element
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
		// ���element���Ĳ�����Χ�ڣ���Ӹ��ڵ㿪ʼ���±����Ƴ�
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
*	\brief ��ȡ��ǰregion��Χ��element
*	\return ���ز������ظ�element��vector
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
*	\brief Debug����
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
*	\brief ��ʼ�����ڵ�
*/
template<typename T>
inline void QuadTree<T>::Node::Initialize(const Rect & rect)
{
	mElements.clear();
	mElements.reserve(MaxElementsInCell);
	mCell = rect;
}

/**
*	\brief ����µ�element
*/
template<typename T>
inline bool QuadTree<T>::Node::Add(const T & element, const Rect & boundingBox)
{
	// ������ڸýڵ�ķ�Χ��
	if (!GetCell().Overlaps(boundingBox))
	{
		return false;
	}

	// ���δ�������ж��Ƿ���Ҫ����
	// ����ֱ�Ӽ��뵽elements��
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

	// ����Ѿ����֣���ֱ����ӵ��������ֽڵ���
	for (auto& child : mChilds)
	{
		child->Add(element, boundingBox);
	}
	return true;
}

/**
*	\brief �ӵ�ǰ�ڵ���ɾ��ָ����element
*	\param boundingBox ��element��rect��Χ
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
		// δ������ֱ�Ӵ�elements��ɾ��
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
	{	// ����Ѿ����֣�������ӽڵ����ɾ������
		bool removed = false;
		for (auto& child : mChilds)
		{
			removed |= child->Remove(element, boundingBox);
		}
		// ������ɾ��֮���Ҹýڵ��ӽڵ�ΪҶ�ӽڵ㣬�����Ƿ���Ҫ�ϲ�
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
*	\brief ��ȡ��ǰ�ڵ��·�Χ�ڵ�element
*	\param region ��Χ��rect
*	\param result ��Ż�ȡ��element
*/
template<typename T>
inline void QuadTree<T>::Node::GetElements(const Rect & region, std::set<T>& result)const
{
	if (!GetCell().Overlaps(region))
	{
		return;
	}

	// ����Ѿ��������4���ӽڵ��л�ȡ
	if (IsSplit())
	{
		for (auto& child : mChilds)
		{
			child->GetElements(region, result);
		}
	}
	else
	{	// ������ǰ�ڵ㱣���element�����ж��Ƿ������ȡ����
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
*	\brief ��ȡ��ǰ�ڵ��element����
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
*	\brief ���Ƶ�ǰquadTree�Ļ���״̬����������element��rect
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
*	\brief �ýڵ��Ƿ��Ѿ�����
*
*	���Ա�֤4�����ֱ�Ȼȫ��ͬʱ���ֻ����ͷ�
*/
template<typename T>
inline bool QuadTree<T>::Node::IsSplit() const
{
	return mChilds[0] != nullptr;
}

/**
*	\brief ���ָýڵ�
*
*	���Ա�֤4�����ֱ�Ȼȫ��ͬʱ���ֻ����ͷ�
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

	// �������ֽڵ�
	mChilds[0] = std::unique_ptr<Node>(new Node( Rect(ltPos, cellSizeInv2)));
	mChilds[1] = std::unique_ptr<Node>(new Node( Rect(Point2(centerPos.x, ltPos.y), cellSizeInv2)));
	mChilds[2] = std::unique_ptr<Node>(new Node( Rect(Point2(ltPos.x, centerPos.y), cellSizeInv2)));
	mChilds[3] = std::unique_ptr<Node>(new Node( Rect(centerPos, cellSizeInv2)));

	// ����ǰ�ڵ��element���浽���ֽڵ�
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
*	\brief ���Ѿ����ֵĽ��ϲ�ΪҶ�ڵ�
*
*	��ִ��remove�����ǻ���øú���
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
