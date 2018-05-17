#include"gui\widget\multGrid.h"
#include"gui\widget\widget.h"
#include"gui\widget\frame.h"

#include<algorithm>
#include<numeric>

namespace gui
{
Grid::Grid():
	mRows(0),
	mCols(0),
	mRowsHeight(),
	mColsWidth(),
	mGridItems(0),
	mNamedChilds()
{
}

Grid::Grid(int row, int col) :
	mRows(row),
	mCols(col),
	mRowsHeight(),
	mColsWidth(),
	mGridItems(row * col),
	mNamedChilds()
{
	mRowsFactory.resize(row, 1.0f);
	mColsFactory.resize(col, 1.0f);

	ConnectSignal<gui::EVENT_REQUEST_PLACEMENT>(
		std::bind(&Grid::RequesetPlacement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
		gui::Dispatcher::back_pre_child);
}

Grid::~Grid()
{
	mNamedChilds.clear();
	/**
	*	临时措施：设置每个child的parent为nullptr,因为同时维护智能指针
	*	和普通指针，不久之后将要修改
	*/
	for (auto& childs : mGridItems)
	{
		for (auto& child : childs)
		{
			if (child.GetWidget() != nullptr)
				child.GetWidget()->SetParent(nullptr);
		}
	}
	mGridItems.clear();
}

void Grid::Update()
{
	ForEachChildren([&](Children& child) {
		auto widget = child.GetWidget();
		if (widget != nullptr)
		{
			widget->Update();
		}
		return true;
	});
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
		return;
	
	if (!mGridItems.empty())
		Debug::Warning("Resizing a non-empty grid");
	
	mRows = row;
	mCols = col;
	mRowsHeight.resize(row, 0);
	mColsWidth.resize(col, 0);
	mRowsFactory.resize(row, 1.0f);
	mColsFactory.resize(col, 1.0f);
	mGridItems.resize(row * col);
}

void Grid::SetRowsFactory(const std::vector<float> factoies)
{
	if (factoies.size() > mRows)
	{
		Debug::Error("The count '" + std::to_string(factoies.size()) +
			" of row factory is invalid.");
		return;
	}
	mRowsFactory = factoies;
}

void Grid::SetColsFactory(const std::vector<float> factoies)
{
	if (factoies.size() > mRows)
	{
		Debug::Error("The count '" + std::to_string(factoies.size()) +
			" of col factory is invalid.");
		return;
	}
	mColsFactory = factoies;
}

/**** **** **** **** *** Child *** **** **** **** ****/

void Grid::ForEachChildren(std::function<bool(Grid::Children&child)> func)
{
	for (auto& gridItems : mGridItems)
	{
		for (auto& child : gridItems)
		{
			if (!func(child))
				return;
		}
	}
}

void Grid::ForEachChildren(std::function<bool(const Grid::Children&child)> func) const
{
	for (const auto& gridItems : mGridItems)
	{
		for (const auto& child : gridItems)
		{
			if (!func(child))
				return;
		}
	}
}

/**
*	\brief 计算当前网格的最佳大小
*/
Size Grid::GetGridItemBestSize(int row, int col) const
{
	Size bestSize(0, 0);
	auto& gridItem = GetGridItem(row, col);
	for (const auto& child : gridItem)
	{
		const Size curSize = child.GetBestSize();
		bestSize.width  = std::max(bestSize.width,  curSize.width);
		bestSize.height = std::max(bestSize.height, curSize.height);
	}
	return bestSize;
}

/**
*	\brief 请求减少一个GridItem宽度，这会尝试减小每个属下的Children
*/
void Grid::RequestReduceGridItemWidth(GridItem & item, const int maxnumWidth)
{
	for (auto& child : item)
	{
		const Size size = child.GetBestSize();
		if(size.width > maxnumWidth)
			RequestReduceCellWidth(child, maxnumWidth);
	}
}

/**
*	\brief 请求减少一个GridItem高度，这会尝试减小每个属下的Children
*/
void Grid::RequestReduceGridItemHeight(GridItem & item, const int maxnumHeight)
{
	for (auto& child : item)
	{
		const Size size = child.GetBestSize();
		if (size.height > maxnumHeight)
			RequestReduceCellHeight(child, maxnumHeight);
	}
}

/**
*	\brief 设置指定网格的子节点
*	\param widget  添加的wiget
*	\param row,col 指定的网格
*	\param flag    子节点的对齐模式
*/
void Grid::SetChildren(const WidgetPtr& widget, int row, int col, const unsigned int flag,int borderSize)
{
	Debug::CheckAssertion(row < mRows && col < mCols);
	Debug::CheckAssertion(flag & ALIGN_VERTICAL_MASK,  "The vertical align is invalid");
	Debug::CheckAssertion(flag & ALIGN_HORIZONTAL_MASK,"The horizontl align is invalid");

	// 先找到是否存在同名Chdren，如果有则直接替换,否则创建一个
	Children* findChild = GetChildren(widget->GetID());
	Children& children = findChild != nullptr? *findChild : *CreateChildren(row, col);
	if (children.GetWidget() != nullptr)
	{
		Debug::Warning(string("The childe '" + children.GetID() + "'will be replaced."));
		children.SetWidget(nullptr);	// use shard_ptr
	}

	children.SetFlag(flag);
	children.SetWidget(widget);
	children.SetBorderSize(borderSize);
	if (children.GetWidget() != nullptr)
	{
		children.GetWidget()->SetGridPos({ row, col });
		children.GetWidget()->SetParent(this);
	}

	// 如果存在名字则添加到name mapping
	if (widget->GetID() != "")
	{
		mNamedChilds[widget->GetID()] = &children;
	}
}

/**
*	\brief 移除指定网格的所有子节点
*	\param row,col 指定的网格
*/
void Grid::RemoveChildren(int row, int col)
{
	Debug::CheckAssertion(row < mRows && col < mCols);

	GridItem& childs = GetGridItem(row, col);
	for (auto& child : childs)
	{
		if (child.GetWidget() != nullptr) {
			child.GetWidget()->SetGridPos({ -1, -1 });
			child.SetWidget(nullptr);
		}
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
		findedChildren->second->Clear();
	}
	else
	{
		ForEachChildren([&](Children& child) {
			if (child.GetID() == id)
				child.Clear();
			return true;
		});
	}
}

void Grid::RemoveAllChildren()
{
	ForEachChildren([&](Children& child) {
		if (child.GetWidget() != nullptr)
			child.Clear();

		return true;
	});
}

Grid::Children* Grid::GetChildren(const WidgetPtr& widget)
{
	Children* ret = nullptr;
	ForEachChildren([&](Children& child) {
		if (child.GetWidget() == widget)
		{
			ret = &child;
		}
		return true;
	});
	return ret;
}

Grid::Children * Grid::GetChildren(const std::string & id)
{
	auto& childIt = mNamedChilds.find(id);
	if (childIt != mNamedChilds.end())
		return childIt->second;

	return nullptr;
}

Grid::Children* Grid::CreateChildren(int row, int col)
{
	GridItem& gridItem = GetGridItem(row, col);
	gridItem.emplace_back();

	// Set a limit
	if (gridItem.size() > 64)
		Debug::Error("The size of grid item is too much.");

	return &gridItem.back();
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

/**
*	\brief 将指定网格的Widget移到最上方
*/
void Grid::TopChildren(Widget & widget, int row, int col)
{
	GridItem& gridItem = GetGridItem(row, col);
	for (auto it = gridItem.begin(); it != gridItem.end(); ++it)
	{
		auto& child = *it;
		if (child.GetWidget() && child.GetWidget().get() == &widget)
		{
			Children newChild = child;
			gridItem.push_back(newChild);
			gridItem.erase(it);
			break;
		}
	}
}

Grid::Children::Children():
	mFlag(0),
	mWidget(nullptr),
	mBorderSize(0)
{
}

const string Grid::Children::GetID() const
{
	string result;
	if (mWidget != nullptr)
	{
		result = mWidget->GetID();
	}
	return result;
}

void Grid::Children::InitLayout()
{
	Debug::CheckAssertion(mWidget != nullptr);

	if (mWidget->GetVisibility())
	{
		mWidget->InitLayout();
	}
}

void Grid::Children::Place(const Point2& pos, const Size& size)
{
	if (mWidget == nullptr || !mWidget->GetVisibility())
	{
		return;
	}
	Point2 adjustedPos(pos);
	Size   adjustedSize(size);

	// 调整边界距离
	if (mBorderSize > 0)
	{
		if (mFlag & BORDER_TOP)
		{
			adjustedPos.y += mBorderSize;
			adjustedSize.height -= mBorderSize;
		}
		if (mFlag & BORDER_LEFT)
		{
			adjustedPos.x += mBorderSize;
			adjustedSize.width -= mBorderSize;
		}
		if (mFlag & BORDER_BOTTOM)
		{
			adjustedSize.height -= mBorderSize;
		}
		if (mFlag & BORDER_RIGHT)
		{
			adjustedSize.width -= mBorderSize;
		}
	}

	const Size bestSize = mWidget->GetBestSize();
	if (size <= bestSize)
	{
		mWidget->Place(adjustedPos, adjustedSize);
	}

	// 当设置大小大于bestSize时，则需要根据对齐方式调整位子
	// 先计算出位置，在根据wantedPos做偏移
	Point2 widgetPos = adjustedPos;
	Size widgetSize = Size(std::min(size.width, bestSize.width), std::min(size.height, bestSize.height));

	int vertialFlag = mFlag & ALIGN_VERTICAL_MASK;
	if (vertialFlag & ALIGN_VERTICAL_TOP)
	{
		// 默认已经顶部对齐,do nothing
		Debug::DebugString("The widget id:" + GetID() + " aligned to vertical top.");
	}
	else if (vertialFlag & ALIGN_VERTICAL_CENTER)
	{
		widgetPos.y += (size.height - widgetSize.height) / 2;
		Debug::DebugString("The widget id:" + GetID() + " aligned to vertical center.");
	}
	else if (vertialFlag & ALIGN_VERTICAL_BOTTOM)
	{
		widgetPos.y += (size.height - widgetSize.height);
		Debug::DebugString("The widget id:" + GetID() + " aligned to vertical bottom.");
	}
	else
	{
		Debug::Die("Invaild vertical align parameter.");
	}

	int horizontalFlag = mFlag & ALIGN_HORIZONTAL_MASK;
	if (horizontalFlag & ALIGN_HORIZONTAL_LEFT)
	{
		// 默认已经顶部对齐,do nothing
		Debug::DebugString("The widget id:" + GetID() + " aligned to horizontal top.");
	}
	else if (horizontalFlag & ALIGN_HORIZONTAL_CENTER)
	{
		widgetPos.x += (size.width - widgetSize.width) / 2;
		Debug::DebugString("The widget id:" + GetID() + " aligned to horizontal center.");
	}
	else if (horizontalFlag & ALIGN_HORIZONTAL_RIGHT)
	{
		widgetPos.x += (size.width - widgetSize.width) ;
		Debug::DebugString("The widget id:" + GetID() + " aligned to horizontal bottom.");
	}
	else
	{
		Debug::Die("Invaild horizontal align parameter.");
	}


	widgetPos += mWidget->GetWantedPositon();
	mWidget->Place(widgetPos, widgetSize);
}

Size Grid::Children::GetBorderSpace()const
{
	Size size(0, 0);
	if (mBorderSize > 0)
	{
		if (mFlag & BORDER_TOP)
		{
			size.height += mBorderSize;
		}
		if (mFlag & BORDER_LEFT)
		{
			size.width += mBorderSize;
		}
		if (mFlag & BORDER_BOTTOM)
		{
			size.height += mBorderSize;
		}
		if (mFlag & BORDER_RIGHT)
		{
			size.width += mBorderSize;
		}
	}
	return size;
}

void Grid::Children::Clear()
{
	if (mWidget != nullptr &&
		mWidget->GetWidgetType() == WIDGET_TYPE::WIDGET_FRAME)
	{
		auto& frame = dynamic_cast<Frame&>(*mWidget);
		frame.RemoveAllChildrens();
	}
	mWidget->SetGridPos({ -1, -1 });
	mWidget = nullptr;
}

/**
*	\brief 获取该子节点的最佳大小
*
*	如果不存在widget则直接返回
*/
Size Grid::Children::GetBestSize()const
{
	if (mWidget == nullptr)
	{
		return GetBorderSpace();
	}

	if (!mWidget->GetVisibility())
	{
		return Size(0, 0);
	}

	// 最佳大小为 Widget的最佳大小 + BorderSpace + wantedPos(位置偏移）
	const Size posOffset(
		mWidget->GetWantedPositon().x,
		mWidget->GetWantedPositon().y
	);
	return  mWidget->GetBestSize() + GetBorderSpace() + posOffset;
}

/**** **** **** **** *** Layout *** **** **** **** ****/

void Grid::InitLayout()
{
	Widget::InitLayout();

	ForEachChildren([&](Children& child) {
		child.InitLayout();
		return true;
	});
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
			auto& gridItem = GetGridItem(row, col);
			for (auto& child : gridItem)
			{
				WidgetPtr widget = child.GetWidget();
				if (widget != nullptr)
					child.Place(pos, size);
			}
			pos.x += mColsWidth[col];
		}
		pos.x = origin.x;
		pos.y += mRowsHeight[row];
	}
}

/**
*	\brief 布局所有子节点
*/
void Grid::Layout(const Point2& origin)
{
	Point2 pos = origin;
	for (int row = 0; row < mRows; row++)
	{
		for (int col = 0; col < mCols; col++)
		{
			const Size size(mColsWidth[col], mRowsHeight[row]);
			auto& gridItem = GetGridItem(row, col);
			for (auto& child : gridItem)
			{
				WidgetPtr widget = child.GetWidget();
				if (widget != nullptr)
					child.Place(pos, size);
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

	if (mRows == 0 || mCols == 0)
		return;

	Size bestSize = CalculateBestSize();
	Debug::CheckAssertion(mRowsHeight.size() == mRows);
	Debug::CheckAssertion(mColsWidth.size() == mCols);

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
	auto ResizeGridCell = [](int diff, std::vector<int>& gridVecot, const std::vector<float>& factory){
		const float invSumFactory = 1.0f / std::accumulate(
			factory.begin(), factory.end(), 0.0f);					// 每个网格的增长因子

		for (int i = 0; i < gridVecot.size(); i++)
		{
			int value = (int)(diff * factory[i] * invSumFactory);	
			gridVecot[i] += value;
		}
	};
	if (size.width > bestSize.width)
	{
		ResizeGridCell(size.width - bestSize.width, mColsWidth, mColsFactory);
	}
	if (size.height > bestSize.height)
	{
		ResizeGridCell(size.height - bestSize.height, mRowsHeight, mRowsFactory);
	}
	LayoutChildren(pos);
}

/**
*	\brief 刷新当前位置，当网格数改变时应该手动调用
*/
void Grid::RefreshPlace()
{
	const Size size = GetSize();
	const Point2 pos = GetPositon();
	Place(pos, size);
}

void Grid::SetPosition(const Point2& position)
{
	Widget::SetPosition(position);

	ForEachChildren([&](Children& child) {
		if (child.GetWidget() != nullptr)
		{
			child.GetWidget()->SetPosition(position);
		}
		return true;
	});
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
			//Size bestSize = GetChildren(row, col).GetBestSize();
			Size bestSize = GetGridItemBestSize(row, col);
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

Size Grid::ReCalculateBestSize()
{
	Size size = CalculateBestSize();
	SetLayoutSize(size);
	return size;
}

void Grid::ClearLuaCallBack()
{
	ForEachChildren([&](Children& child) {
		auto widget = child.GetWidget();
		if (widget != nullptr)
			widget->ClearLuaCallBack();

		return true;
	});
}

/**
*	\brief 减少当前宽度
*
*	当减少到的宽度小于bestSize时，会调用requestReduceSize
*/
void Grid::ReduceWidth(const int maxnumWidth)
{
	const Size bestSize = GetBestSize();
	if (bestSize.width <=  maxnumWidth)
	{
		return;
	}
	// reduce width friendly
	RequestReduceWidth(maxnumWidth);
	
	const Size bestSizeAfterReduce = ReCalculateBestSize();
	if (bestSizeAfterReduce.width <=  maxnumWidth)
	{
		return;
	}
	// 请求较少空间失败
	Debug::Error("Failed to reduce width.");
}

void Grid::RequestReduceWidth(const int maxnumWidth)
{
	Size bestSize = GetBestSize();
	if (bestSize.width <= maxnumWidth)
	{
		return;
	}
	
	/**
	*	当bestSize 大于请求的大小时，需要进行网格缩减
	*	即遍历每一列网格，将每个子网格大小减少到合适程度
	*/
	const int reduceWidth = bestSize.width - maxnumWidth;
	int appendWidth = 0;			
	for (int col = 0; col < mCols; col++)
	{
		if (reduceWidth - appendWidth >= mColsWidth[col])
		{
			continue;
		}

		const int wantedWidth = mColsWidth[col] - (reduceWidth - appendWidth);
		int width = RequestReduceColWidth(col, wantedWidth);
		if (width < mColsWidth[col] )
		{
			int reduction = mColsWidth[col] - width;
			appendWidth += reduction;
			bestSize.width -= reduction;
		}

		if (bestSize.width <= maxnumWidth)
		{
			break;
		}
	}
	SetLayoutSize(CalculateBestSize());
}

void Grid::DemandReduceWidth(const int maxnumWidth)
{

}

/**
*	\brief 请求减少指定网格宽度
*/
int Grid::RequestReduceColWidth(int col, const int maxnumWidth)
{
	int requiredWidth = 0;
	for (size_t row = 0; row < mRowsHeight.size(); row++)
	{
		//Children& cell = GetChildren(row, col);
		//RequestReduceCellWidth(cell, maxnumWidth);
		//const Size size = cell.GetBestSize();

		GridItem& gridItem = GetGridItem(row, col);
		RequestReduceGridItemWidth(gridItem, maxnumWidth);
		const Size size = GetGridItemBestSize(row, col);

		if (requiredWidth == 0 || size.width > requiredWidth)
		{
			requiredWidth = size.width;
		}
	}

	return requiredWidth;
}

void Grid::RequestReduceCellWidth(Children & child, const int maxnumWidth)
{
	if (child.GetWidget() != nullptr)
	{
		if (!child.GetWidget()->GetVisibility())
		{
			return;
		}

		child.GetWidget()->RequestReduceWidth(maxnumWidth - child.GetBorderSpace().width);
	}
}

/**
*	\brief 减少当前高度
*
*	当减少到的宽度小于bestSize时，会调用requestReduceSize
*/
void Grid::ReduceHeight(const int maxnumHeight)
{
	const Size bestSize = GetBestSize();
	if (bestSize.height <= maxnumHeight)
	{
		return;
	}
	// reduce width friendly
	RequestReduceHeight(maxnumHeight);

	const Size bestSizeAfterReduce = ReCalculateBestSize();
	if (bestSizeAfterReduce.height <= maxnumHeight)
	{
		return;
	}
	// 请求较少空间失败
	Debug::Error("Failed to reduce width.");
}

void Grid::RequestReduceHeight(const int maxnumHeight)
{
	Size bestSize = GetBestSize();
	if (bestSize.height <= maxnumHeight)
	{
		return;
	}

	/**
	*	当bestSize 大于请求的大小时，需要进行网格缩减
	*	即遍历每一列网格，将每个子网格大小减少到合适程度
	*/
	const int reduceHeight = bestSize.height - maxnumHeight;
	int appendHeight = 0;
	for (int row = 0; row < mRows; row++)
	{
		if (reduceHeight - appendHeight >= mRowsHeight[row])
		{
			continue;
		}

		const int wantedHeight = mRowsHeight[row] - (reduceHeight - appendHeight);
		int height = RequestReduceRowHeight(row, wantedHeight);
		if (height < mRowsHeight[row])
		{
			int reduction = mRowsHeight[row] - height;
			appendHeight += reduction;
			bestSize.height -= reduction;
		}

		if (bestSize.height <= maxnumHeight)
		{
			break;
		}
	}
	SetLayoutSize(CalculateBestSize());
}

void Grid::DemandReduceHeight(const int maxnumHeight)
{
}

int Grid::RequestReduceRowHeight(int row, const int maxnumWidth)
{
	int requiredHeight = 0;
	for (int col = 0; col < mCols; col++)
	{
		//Children& cell = GetChildren(row, col);
		//RequestReduceCellHeight(cell, maxnumWidth);
		//const Size size = cell.GetBestSize();

		GridItem& gridItem = GetGridItem(row, col);
		RequestReduceGridItemHeight(gridItem, maxnumWidth);
		const Size size = GetGridItemBestSize(row, col);
		if (requiredHeight == 0 || size.height > requiredHeight)
		{
			requiredHeight = size.height;
		}
	}
	return requiredHeight;
}

void Grid::RequestReduceCellHeight(Children & child, const int maxnumHeight)
{
	if (child.GetWidget() != nullptr)
	{
		if (!child.GetWidget()->GetVisibility())
		{
			return;
		}
		child.GetWidget()->RequestReduceHeight(maxnumHeight);
	}
}

void Grid::ImplDrawChildren(const Point2 & offset)
{
	Debug::CheckAssertion(GetVisibility());
	SetIsDirty(false);

	ForEachChildren([&](Children& child) {
		auto widget = child.GetWidget();
		if (widget == nullptr ||
			!widget->GetVisibility()  ||
			widget->GetReDrawAction() == Widget::ReDrawAction::None){
			return true;
		}

		widget->DrawBackground(offset);
		widget->DrawChildren(offset);
		widget->DrawForeground(offset);
		widget->SetIsDirty(false);

		return true;
	});
}


/**
*	\brief 请求设置问题
*
*	signal函数,在构造时调用signalConnect设置
*/
void Grid::RequesetPlacement(Dispatcher&, const gui::ui_event, bool&handle, bool& halt)
{
	Size size = GetSize();
	Size bestSize = GetBestSize();
	if (size >= bestSize)
	{
		Place(GetPositon(), size);
		handle = true;
		return;
	}
	ReCalculateBestSize();
	
	// 当空间不够需要重新布局
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
	bool ret = false;
	ForEachChildren([&](const Children& child) {
		auto contanierWidget = child.GetWidget();
		if (contanierWidget != nullptr)
		{
			auto result = contanierWidget->HasWidget(widget);
			if (result)
			{
				ret = result;
				return false;
			}
		}
		return true;
	});
	return ret;
}

bool Grid::IsAt(const Point2& pos)const
{
	return false;
}

/**
*	\brief 查找指定坐标下的widget
*
*	遍历每个子节点调用findAt方法
*/
Widget * Grid::FindAt(const Point2 & pos)
{
	for (auto& gridItems : mGridItems)
	{
		auto rit = gridItems.rbegin();
		for (; rit != gridItems.rend(); rit++)
		{
			auto widget = rit->GetWidget();
			if (widget != nullptr && widget->GetVisibility())
			{
				auto findIt = widget->FindAt(pos);
				if (findIt)
					return findIt;
			}
		}
	}
	return nullptr;
}

}