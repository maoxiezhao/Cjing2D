#include "containerBase.h"

namespace gui {

	ContainerBase::ContainerBase() :
		StyledWidget(),
		mGrid(1,1)
	{
		mGrid.SetParent(this);
	}

	void ContainerBase::InitLayout()
	{
		mGrid.InitLayout();
	}

	/**
	*	\brief 减少宽度
	*/
	void ContainerBase::ReduceWidth(const int maxnumWidth)
	{
		Size size = GetBestSize();
		Size gridSize = mGrid.GetBestSize();
		if (maxnumWidth < gridSize.width)
		{
			mGrid.ReduceWidth(maxnumWidth);
			gridSize = mGrid.GetBestSize();
			size.width = gridSize.width;
			size.height = std::max(gridSize.height, size.height);
		}
		else
		{
			size.width = maxnumWidth;
		}
		SetLayoutSize(size);
	}

	/**
	*	\brief 请求减少宽度
	*/
	void ContainerBase::RequestReduceWidth(const int maxnumWidth)
	{
		Size size = GetBestSize();
		Size gridSize = mGrid.GetBestSize();
		if (maxnumWidth < gridSize.width)
		{
			mGrid.RequestReduceWidth(maxnumWidth);
			gridSize = mGrid.GetBestSize();
			size.width = gridSize.width;
			size.height = std::max(gridSize.height, size.height);
		}
		else
		{
			size.width = maxnumWidth;
		}
		SetLayoutSize(size);
	}

	/**
	*	\brief 请求减少高度
	*/
	void ContainerBase::ReduceHeight(const int maxnumHeight)
	{
		Size size = GetBestSize();
		Size gridSize = mGrid.GetBestSize();
		if (maxnumHeight < gridSize.height)
		{
			mGrid.ReduceHeight(maxnumHeight);
			gridSize = mGrid.GetBestSize();
			size.height = gridSize.height;
			size.width = std::max(gridSize.width, size.width);
		}
		else
		{
			size.height = maxnumHeight;
		}
		SetLayoutSize(size);
	}

	/**
	*	\brief 请求减少宽度
	*/
	void ContainerBase::RequestReduceHeight(const int maxnumHeight)
	{
		Size size = GetBestSize();
		Size gridSize = mGrid.GetBestSize();
		if (maxnumHeight < gridSize.height)
		{
			mGrid.RequestReduceHeight(maxnumHeight);
			gridSize = mGrid.GetBestSize();
			size.height = gridSize.height;
			size.width = std::max(gridSize.width, size.width);
		}
		else
		{
			size.height = maxnumHeight;
		}
		SetLayoutSize(size);
	}

	/**
	*	\brief 获取最佳大小
	*/
	Size ContainerBase::GetBestSize() const
	{
		return StyledWidget::GetBestSize();
	}

	/**
	*	\brief 计算最佳大小，最佳大小即网格大小
	*/
	Size ContainerBase::CalculateBestSize() const
	{
		Size result(mGrid.GetBestSize());
		return result;
	}

	/**
	*	\brief 是否可以换行
	*/
	bool ContainerBase::CanWrap() const
	{
		return mGrid.CanWrap() || Widget::CanWrap();
	}

	void ContainerBase::Place(const Point2 & pos, const Size & size)
	{
		StyledWidget::Place(pos, size);
		mGrid.Place(pos, size);
	}

	void ContainerBase::RefreshPlace()
	{
		StyledWidget::RefreshPlace();
		mGrid.RefreshPlace();
	}

	void ContainerBase::LayoutChildren(const Point2 & origin)
	{
		mGrid.LayoutChildren(origin);
	}

	bool ContainerBase::HasWidget(const Widget & widget) const
	{
		return false;
	}

	/**
	*	\brief 绘制子节点
	*/
	void ContainerBase::ImplDrawChildren(const Point2 & offset)
	{
		mGrid.DrawChildren(offset);
	}

	void ContainerBase::InitGrid()
	{
	}

}
