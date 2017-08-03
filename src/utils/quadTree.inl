#include"rectangle.h"

template <typename T>
QuadTree<T>::QuadTree()
{

}

/**
*	\brief Ìí¼Óelement
*/
template<typename T>
inline bool QuadTree<T>::Add(const T & element, const Rect & boundingBox)
{
	if (Contain(element))
		return false;
	if (!boundingBox.overlaps(GetSpace()))
	{
		mElementsOut.insert(element);
		return true;
	}
	if (!mRoot.Add(element, boundingBox))
		return false;
	return true;
}

template<typename T>
QuadTree<T>::Node::Node()
{
}

template<typename T>
inline bool QuadTree<T>::Node::Remove(const T & element)
{
	
	return false;
}
