#include"gui\widget\singleGrid.h"
#include"gui\widget\widget.h"

#include<algorithm>
#include<numeric>

namespace gui
{
	SingleGrid::SingleGrid() :
		mRows(0),
		mCols(0),
		mRowsHeight(),
		mColsWidth(),
		mChilds(0),
		mNamedChilds()
	{
	}

	SingleGrid::SingleGrid(int row, int col) :
		mRows(row),
		mCols(col),
		mRowsHeight(),
		mColsWidth(),
		mChilds(row * col),
		mNamedChilds()
	{
		mRowsFactory.resize(row, 1.0f);
		mColsFactory.resize(col, 1.0f);

		ConnectSignal<gui::EVENT_REQUEST_PLACEMENT>(
			std::bind(&SingleGrid::RequesetPlacement, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
			gui::Dispatcher::back_pre_child);
	}

	SingleGrid::~SingleGrid()
	{
		mNamedChilds.clear();
		/**
		*	临时措施：设置每个child的parent为nullptr,因为同时维护智能指针
		*	和普通指针，不久之后将要修改
		*/
		for (auto& child : mChilds)
		{
			if (child.GetWidget() != nullptr)
			{
				child.GetWidget()->SetParent(nullptr);
			}
		}
		mChilds.clear();
	}

	void SingleGrid::AddRows(int rowCount)
	{
		SetRowCols(mRows + rowCount, mCols);
	}

	void SingleGrid::AddCols(int colCount)
	{
		SetRowCols(mRows, mCols + colCount);
	}

	void SingleGrid::SetCols(int cols)
	{
		if (mCols == cols)
		{
			return;
		}
		SetRowCols(mRows, cols);
	}

	void SingleGrid::SetRows(int rows)
	{
		if (mRows == rows)
		{
			return;
		}
		SetRowCols(rows, mCols);
	}

	void SingleGrid::SetRowCols(int row, int col)
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
		mRowsHeight.resize(row, 0);
		mColsWidth.resize(col, 0);
		mRowsFactory.resize(row, 1.0f);
		mColsFactory.resize(col, 1.0f);
		mChilds.resize(row * col);
	}

	void SingleGrid::SetRowsFactory(const std::vector<float> factoies)
	{
		if (factoies.size() > mRows)
		{
			Debug::Error("The count '" + std::to_string(factoies.size()) +
				" of row factory is invalid.");
			return;
		}
		mRowsFactory = factoies;
	}

	void SingleGrid::SetColsFactory(const std::vector<float> factoies)
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

	/**
	*	\brief 设置指定网格的子节点
	*	\param widget  添加的wiget
	*	\param row,col 指定的网格
	*	\param flag    子节点的对齐模式
	*/
	void SingleGrid::SetChildren(const WidgetPtr& widget, int row, int col, const unsigned int flag, int borderSize)
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
		children.SetBorderSize(borderSize);
		if (children.GetWidget() != nullptr)
		{
			children.GetWidget()->SetParent(this);
		}
	}

	/**
	*	\brief 移除指定网格的子节点
	*	\param row,col 指定的网格
	*/
	void SingleGrid::RemoveChildren(int row, int col)
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
	void SingleGrid::RemoveChildren(const string& id)
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

	SingleGrid::Children* SingleGrid::GetChildren(const WidgetPtr& widget)
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
	void SingleGrid::SetChildrenAlignment(WidgetPtr widget, const unsigned int setflag, const unsigned int modeMask)
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

	SingleGrid::Children::Children() :
		mFlag(0),
		mWidget(nullptr),
		mBorderSize(0)
	{
	}

	const string SingleGrid::Children::GetID() const
	{
		string result;
		if (mWidget != nullptr)
		{
			result = mWidget->GetID();
		}
		return result;
	}

	void SingleGrid::Children::InitLayout()
	{
		Debug::CheckAssertion(mWidget != nullptr);

		if (mWidget->GetVisibility())
		{
			mWidget->InitLayout();
		}
	}

	void SingleGrid::Children::Place(const Point2& pos, const Size& size)
	{
		if (mWidget == nullptr || !mWidget->GetVisibility() )
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
			widgetPos.x += (size.width - widgetSize.width);
			Debug::DebugString("The widget id:" + GetID() + " aligned to horizontal bottom.");
		}
		else
		{
			Debug::Die("Invaild horizontal align parameter.");
		}


		widgetPos += mWidget->GetWantedPositon();
		mWidget->Place(widgetPos, widgetSize);
	}

	Size SingleGrid::Children::GetBorderSpace()const
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

	/**
	*	\brief 获取该子节点的最佳大小
	*
	*	如果不存在widget则直接返回
	*/
	Size SingleGrid::Children::GetBestSize()const
	{
		if (mWidget == nullptr)
		{
			return GetBorderSpace();
		}

		if (!mWidget->GetVisibility())
		{
			return Size(0, 0);
		}

		return  mWidget->GetBestSize() + GetBorderSpace();
	}

	/**** **** **** **** *** Layout *** **** **** **** ****/

	void SingleGrid::InitLayout()
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
	void SingleGrid::LayoutChildren(const Point2& origin)
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
					GetChildren(row, col).Place(pos, size);
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
	void SingleGrid::Layout(const Point2& origin)
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
					GetChildren(row, col).Place(pos, size);
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
	void SingleGrid::Place(const Point2& pos, const Size& size)
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
			return;
		}

		// 当设置的大小大于所需的大小，则调整grid,这里默认所有的网格
		// 增长因子相同
		auto ResizeGridCell = [](int diff, std::vector<int>& gridVecot, const std::vector<float>& factory) {
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
	void SingleGrid::RefreshPlace()
	{
		const Size size = GetSize();
		const Point2 pos = GetPositon();
		Place(pos, size);
	}

	void SingleGrid::SetPosition(const Point2& position)
	{
		Widget::SetPosition(position);

		for (auto& child : mChilds)
		{
			if (child.GetWidget() != nullptr)
			{
				child.GetWidget()->SetPosition(position);
			}
		}
	}

	/**
	*	\brief 计算最佳大小
	*
	*	通过计算每个子节点的最佳大小
	*/
	Size SingleGrid::CalculateBestSize()const
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

	Size SingleGrid::ReCalculateBestSize()
	{
		Size size = CalculateBestSize();
		SetLayoutSize(size);
		return size;
	}

	/**
	*	\brief 减少当前宽度
	*
	*	当减少到的宽度小于bestSize时，会调用requestReduceSize
	*/
	void SingleGrid::ReduceWidth(const int maxnumWidth)
	{
		const Size bestSize = GetBestSize();
		if (bestSize.width <= maxnumWidth)
		{
			return;
		}
		// reduce width friendly
		RequestReduceWidth(maxnumWidth);

		const Size bestSizeAfterReduce = ReCalculateBestSize();
		if (bestSizeAfterReduce.width <= maxnumWidth)
		{
			return;
		}
		// 请求较少空间失败
		Debug::Error("Failed to reduce width.");
	}

	void SingleGrid::RequestReduceWidth(const int maxnumWidth)
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
			if (width < mColsWidth[col])
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

	void SingleGrid::DemandReduceWidth(const int maxnumWidth)
	{

	}

	/**
	*	\brief 请求减少指定网格宽度
	*/
	int SingleGrid::RequestReduceColWidth(int col, const int maxnumWidth)
	{
		int requiredWidth = 0;
		for (size_t row = 0; row < mRowsHeight.size(); row++)
		{
			Children& cell = GetChildren(row, col);
			RequestReduceCellWidth(cell, maxnumWidth);
			const Size size = cell.GetBestSize();

			if (requiredWidth == 0 || size.width > requiredWidth)
			{
				requiredWidth = size.width;
			}
		}

		return requiredWidth;
	}

	void SingleGrid::RequestReduceCellWidth(Children & child, const int maxnumWidth)
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
	void SingleGrid::ReduceHeight(const int maxnumHeight)
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

	void SingleGrid::RequestReduceHeight(const int maxnumHeight)
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

	void SingleGrid::DemandReduceHeight(const int maxnumHeight)
	{
	}

	int SingleGrid::RequestReduceRowHeight(int row, const int maxnumWidth)
	{
		int requiredHeight = 0;
		for (int col = 0; col < mCols; col++)
		{
			Children& cell = GetChildren(row, col);
			RequestReduceCellHeight(cell, maxnumWidth);

			const Size size = cell.GetBestSize();
			if (requiredHeight == 0 || size.height > requiredHeight)
			{
				requiredHeight = size.height;
			}
		}
		return requiredHeight;
	}

	void SingleGrid::RequestReduceCellHeight(Children & child, const int maxnumHeight)
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

	void SingleGrid::ImplDrawChildren(const Point2 & offset)
	{
		Debug::CheckAssertion(GetVisibility());
		SetIsDirty(false);

		for (auto& child : mChilds)
		{
			auto widget = child.GetWidget();
			if (widget == nullptr ||
				!widget->GetVisibility() ||
				widget->GetReDrawAction() == Widget::ReDrawAction::None)
			{
				continue;
			}

			widget->DrawBackground(offset);
			widget->DrawChildren(offset);
			widget->DrawForeground(offset);
			widget->SetIsDirty(false);
		}
	}


	/**
	*	\brief 请求设置问题
	*
	*	signal函数,在构造时调用signalConnect设置
	*/
	void SingleGrid::RequesetPlacement(Dispatcher&, const gui::ui_event, bool&handle, bool& halt)
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

	Widget* SingleGrid::Find(string& id, const bool activited)
	{
		return nullptr;
	}

	const Widget* SingleGrid::Find(string& id, const bool activied)const
	{
		return nullptr;
	}

	bool SingleGrid::HasWidget(const Widget& widget)const
	{
		for (auto& child : mChilds)
		{
			auto contanierWidget = child.GetWidget();
			if (contanierWidget == nullptr)
			{
				continue;
			}

			auto result = contanierWidget->HasWidget(widget);
			if (result)
			{
				return result;
			}
		}
		return false;
	}

	bool SingleGrid::IsAt(const Point2& pos)const
	{
		return false;
	}

	/**
	*	\brief 查找指定坐标下的widget
	*
	*	遍历每个子节点调用findAt方法
	*/
	Widget * SingleGrid::FindAt(const Point2 & pos)
	{
		for (auto& child : mChilds)
		{
			auto widget = child.GetWidget();
			if (widget == nullptr)
			{
				continue;
			}

			auto result = widget->FindAt(pos);
			if (result)
			{
				return result;
			}
		}
		return nullptr;
	}

}