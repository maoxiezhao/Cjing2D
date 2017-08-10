#include"grid.h"
#include"widget.h"

#include<algorithm>
#include<numeric>

namespace gui
{

Grid::Grid(int row, int col) :
	mRows(row),
	mCols(col),
	mRowsHeight(),
	mColsWidth(),
	mChilds(row * col),
	mNamedChilds()
{
	ConnectSignal<gui::EVENT_REQUEST_PLACEMENT>(
		std::bind(&Grid::RequesetPlacement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
		gui::Dispatcher::back_pre_child);
}

Grid::~Grid()
{
	mNamedChilds.clear();
	mChilds.clear();
}

void Grid::AddRows(int rowCount )
{
	SetRowCols(mRows + rowCount, mCols);
}

void Grid::AddCols(int colCount )
{
	SetRowCols(mRows, mCols + colCount);
}

void Grid::SetCols(int cols)
{
	if (mCols == cols)
	{
		return;
	}
	SetRowCols(mRows, cols);
}

void Grid::SetRows(int rows)
{
	if (mRows == rows)
	{
		return;
	}
	SetRowCols(rows, mCols);
}

void Grid::SetRowCols(int row, int col)
{
	if (mRows == row && mCols == col)
	{
		return;
	}
	if (!mChilds.empty())
	{
		Debug::Warning("Resizing a non-empty grid");
	}
	mRows = row;
	mCols = col;
	mChilds.resize(row * col);
}

/**** **** **** **** *** Child *** **** **** **** ****/

/**
*	\brief 设置指定网格的子节点
*	\param widget  添加的wiget
*	\param row,col 指定的网格
*	\param flag    子节点的对齐模式
*/
void Grid::SetChildren(const WidgetPtr& widget, int row, int col, const unsigned int flag)
{
	Debug::CheckAssertion(row < mRows && col < mCols);
	Debug::CheckAssertion(flag & ALIGN_VERTICAL_MASK);
	Debug::CheckAssertion(flag & ALIGN_HORIZONTAL_MASK);

	Children& children = GetChildren(row, col);
	if (children.GetWidget() != nullptr)
	{
		Debug::Warning(string("The childe '" + children.GetID() + "'will be replaced."));
		children.SetWidget(nullptr);	// use shard_ptr
	}

	children.SetFlag(flag);
	children.SetWidget(widget);
	if (children.GetWidget() != nullptr)
	{
		children.GetWidget()->SetParent(this);
	}
}

/**
*	\brief 移除指定网格的子节点
*	\param row,col 指定的网格
*/
void Grid::RemoveChildren(int row, int col)
{
	Debug::CheckAssertion(row < mRows && col < mCols);

	Children& children = GetChildren(row, col);
	if (children.GetWidget() != nullptr)
	{
		children.SetWidget(nullptr);
	}
}

/**
*	\brief 移除指定网格的子节点
*	\param row,col 指定的网格
*/
void Grid::RemoveChildren(const string& id)
{
	auto& findedChildren = mNamedChilds.find(id);
	if (findedChildren != mNamedChilds.end())
	{
		findedChildren->second.SetWidget(nullptr);
	}
	else
	{
		for (auto& child : mChilds)
		{
			if (child.GetID() == id)
			{
				child.SetWidget(nullptr);
			}
		}
	}
}

Grid::Children* Grid::GetChildren(const WidgetPtr& widget)
{
	for (auto& child : mChilds)
	{
		if (child.GetWidget() == widget)
			return &child;
	}
	return nullptr;
}

/**
*	\brief 设置指定网格子节点的对齐方式
*	\param flag      对齐方式枚举值
*	\param modeMaks  对齐方式的种类
*/
void Grid::SetChildrenAlignment(WidgetPtr widget, const unsigned int setflag, const unsigned int modeMask)
{
	Children* children = GetChildren(widget);
	if (children == nullptr)
	{
		return;
	}
	unsigned int flag = children->GetFlag();
	flag &= ~modeMask;
	flag |= setflag;
	children->SetFlag(flag);
}

Grid::Children::Children()
{
}

const string Grid::Children::GetID() const
{
	return string();
}

void Grid::Children::InitLayout()
{
}

/**
*	\brief 获取该子节点的最佳大小
*/
Size Grid::Children::GetBestSize()const
{
	return Size(0,0);
}

void Grid::Children::SetFlag(const unsigned int flag)
{
}

unsigned int Grid::Children::GetFlag() const
{
	return 0;
}

void Grid::Children::SetWidget(const WidgetPtr & widget)
{
}

WidgetPtr Grid::Children::GetWidget()
{
	return WidgetPtr();
}

const WidgetPtr Grid::Children::GetWidget() const
{
	return WidgetPtr();
}

/**** **** **** **** *** Layout *** **** **** **** ****/

void Grid::InitLayout()
{
	Widget::InitLayout();
	for (auto& child : mChilds)
	{
		child.InitLayout();
	}
}

/**
*	\brief 布局所有子节点
*/
void Grid::LayoutChildren(const Point2& origin)
{
	Point2 pos = origin;
	for (int row = 0; row < mRows; row++)
	{
		for (int col = 0; col < mCols; col++)
		{
			const Size size(mColsWidth[col], mRowsHeight[row]);
			WidgetPtr widget = GetChildren(row, col).GetWidget();
			if (widget != nullptr)
			{
				widget->Place(pos, size);
			}
			pos.x += mColsWidth[col];
		}
		pos.x = origin.x;
		pos.y += mRowsHeight[row];
	}
}

/**
*	\brief 设置对应位置,并设置对应大小
*
*    设置大小和位置会重新布局该容器下的所有子节点，同时
*	 设置的大小必须大于等于best_size
*/
void Grid::Place(const Point2& pos, const Size& size)
{
	Widget::Place(pos, size);

	Debug::CheckAssertion(mRowsHeight.size() == mRows);
	Debug::CheckAssertion(mColsWidth.size() == mCols);

	Size bestSize = GetBestSize();
	if (bestSize == size)
	{
		LayoutChildren(pos);
		return;
	}
	if (bestSize.width > size.width || bestSize.height > size.height)
	{
		Debug::Error("Faild to place grid,we have not enough space.");
		return ;
	}

	// 当设置的大小大于所需的大小，则调整grid,这里默认所有的网格
	// 增长因子相同
	auto ResizeGridCell = [](int diff, std::vector<int>& gridVecot){
		const int factor = 1;					// 每个网格的增长因子
		int count = gridVecot.size() * factor;	// 网格个数
		int growBycell = diff / count;			// 每个网格增长因子

		for (auto& val : gridVecot)
		{
			val += growBycell;
		}
	};
	if (size.width > bestSize.width)
	{
		ResizeGridCell(size.width - bestSize.width, mColsWidth);
	}
	if (size.height > bestSize.height)
	{
		ResizeGridCell(size.height - bestSize.height, mRowsHeight);
	}
	
	LayoutChildren(pos);
}

void Grid::Move(const Point2& offset)
{
}

void Grid::Move(const int xoffset, const int yoffset)
{
}

/**
*	\brief 计算最佳大小
*
*	通过计算每个子节点的最佳大小
*/
Size Grid::CalculateBestSize()const
{
	mColsWidth.clear();
	mColsWidth.resize(mCols, 0);
	mRowsHeight.clear();
	mRowsHeight.resize(mRows, 0);

	for (int row = 0; row < mRows; row++)
	{
		for (int col = 0; col < mCols; col++)
		{
			Size bestSize = GetChildren(row, col).GetBestSize();
			if (bestSize.width > mColsWidth[col])
			{
				mColsWidth[col] = bestSize.width;
			}
			if (bestSize.height > mRowsHeight[row])
			{
				mRowsHeight[row] = bestSize.height;
			}
		}
	}
	// 求和计算总的大小
	Size bestSize(std::accumulate(mColsWidth.begin(), mColsWidth.end(), 0),
				  std::accumulate(mRowsHeight.begin(), mRowsHeight.end(), 0));

	return bestSize;
}

void Grid::ImplDrawBackground()
{
}

void Grid::ImplDrawForeground()
{
}

void Grid::ImplDrawChildren()
{
}

/**
*	\brief 请求设置问题
*
*	signal函数,在构造时调用signalConnect设置
*/
void Grid::RequesetPlacement(Dispatcher&, const gui::ui_event, bool&handle, bool& halt)
{

}

Widget* Grid::Find(string& id, const bool activited)
{
	return nullptr;
}

const Widget* Grid::Find(string& id, const bool activied)const
{
	return nullptr;
}

bool Grid::HasWidget(const Widget& widget)const
{
	return false;
}

bool Grid::IsAt(const Point2& pos)const
{
	return false;
}

}