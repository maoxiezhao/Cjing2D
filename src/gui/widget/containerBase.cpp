#include "containerBase.h"

namespace gui {

ContainerBase::ContainerBase()
{
}

void ContainerBase::InitLayout()
{
}

void ContainerBase::ReduceWidth(const int maxnumWidth)
{
}

void ContainerBase::RequestReduceWidth(const int maxnumWidth)
{
}

void ContainerBase::DemandReduceWidth(const int maxnumWidth)
{
}

void ContainerBase::ReduceHeight(const int maxnumHeight)
{
}

void ContainerBase::RequestReduceHeight(const int maxnumHeight)
{
}

void ContainerBase::DemandReduceHeight(const int maxnumHeight)
{
}

Size ContainerBase::GetBestSize() const
{
	return Size();
}

Size ContainerBase::CalculateBestSize() const
{
	return Size();
}

bool ContainerBase::CanWrap() const
{
	return false;
}

void ContainerBase::Place(const Point2 & pos, const Size & size)
{
}

void ContainerBase::LayoutChildren(const Point2 & origin)
{
}

bool ContainerBase::HasWidget(const Widget & widget) const
{
	return false;
}

void ContainerBase::ImplDrawChildren(const Point2 & offset)
{
}

void ContainerBase::InitGrid()
{
}

}