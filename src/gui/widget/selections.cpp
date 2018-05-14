#include "selections.h"
#include "gui\widget\selections_private.h"
#include "gui\widget\styledwidget.h"
#include "gui\widget\selectableItem.h"
#include "gui\widget\toggleButton.h"

namespace gui
{

Selections::Selections():
	mSelectedItemCount(0),
	mLastSelectedItem(-1)
{
	ConnectSignal<ui_event::UI_EVENT_KEY_DOWN>(
		std::bind(&Selections::SignalHanderKeyDown, this, std::placeholders::_2, std::placeholders::_3, std::placeholders::_5));
}

Selections::Selections(Placement * placement, 
			SelectAction* selectAction,
			MaxmunSelection* maxmumSelection,
			MinmumSelection* minmumSelection):

	mPlacement(std::unique_ptr<Placement>(placement)),
	mSelectionAction(std::unique_ptr<SelectAction>(selectAction)),
	mMaxmumSelection(std::unique_ptr<MaxmunSelection>(maxmumSelection)),
	mMinmumSelection(std::unique_ptr<MinmumSelection>(minmumSelection)),
	mSelectedItemCount(0),
	mLastSelectedItem(-1),
	mItemCols(0),
	mItemRows(1)
{
	mPlacement->SetSelections(this);
	mMaxmumSelection->SetSelections(this);
	mSelectionAction->SetSelections(this);
	mMinmumSelection->SetSelections(this);

	ConnectSignal<ui_event::UI_EVENT_KEY_DOWN>(
		std::bind(&Selections::SignalHanderKeyDown, this, std::placeholders::_2, std::placeholders::_3, std::placeholders::_5));
}

Selections::~Selections()
{
}

/**
*	\brief 按顺序添加一个新的item
*/
void Selections::AddItem(WidgetPtr widget)
{
	if (widget == nullptr)
	{
		return;
	}
	Child* item = new Child;
	item->mOrderedIndex = mItems.size();
	item->mGrid.SetRowCols(1, 1);
	item->mGrid.SetChildren(widget, 0, 0, ALIGN_HORIZONTAL_CENTER | ALIGN_VERTICAL_TOP, 0);
	item->mGrid.Place(Point2(0, 0), widget->GetSize());
	item->mGrid.SetParent(this);

	mPlacement->AddItem(*widget);
	mSelectionAction->InitItem(widget.get());
	mItems.push_back(std::unique_ptr<Child>(item));
}

/**
*	\brief 更改某个选择项的选择状态
*/
void Selections::SelectItem(const unsigned int index, bool selected)
{
	Debug::CheckAssertion(index < static_cast<unsigned int>(GetItemCount()), "Invalid index in selecting item.");
	Debug::CheckAssertion(mMaxmumSelection != nullptr, "The maxmumselection is empty.");
	Debug::CheckAssertion(mMinmumSelection != nullptr, "The minmumselection is empty.");

	if (selected && !IsSelected(index))
	{	// 增加selected item
		mMaxmumSelection->SelectItem(index);
	}
	else if (IsSelected(index))
	{	// 减少selected item
		if (!mMinmumSelection->DeSelectItem(index))
		{
			Debug::Warning("DeSelect Item Failed in selecting item.");
		}
	}
}

int Selections::GetItemCount() const
{
	return static_cast<int>(mItems.size());
}

int Selections::GetSelectedItemCount() const
{
	return mSelectedItemCount;
}

/**
*	\brief 返回选择的项的index
*	\return -1 如果不存在已选择项
*/
int Selections::GetSelectedItem()
{
	if (GetSelectedItemCount() == 0)
	{
		return -1;
	}
	else if (mLastSelectedItem != -1 && mLastSelectedItem < static_cast<int>(GetItemCount()))
	{
		auto&grid = mItems[mLastSelectedItem]->mGrid;
		auto widget = std::dynamic_pointer_cast<SelectableItem>(grid.GetWidget(0, 0));
		if (widget != nullptr && widget->IsSelected() && mItems[mLastSelectedItem]->mSelected)
		{
			return mLastSelectedItem;
		}
	}
	else
	{
		for (int i = GetItemCount() - 1; i >= 0; i--)
		{
			auto&grid = mItems[i]->mGrid;
			auto widget = std::dynamic_pointer_cast<SelectableItem>(grid.GetWidget(0, 0));
			if (widget != nullptr && widget->IsSelected() && mItems[i]->mSelected)
			{
				return i;
			}
		}
	}
	return -1;
}

SingleGrid & Selections::GetItemGrid(const unsigned int index)
{
	Debug::CheckAssertion(index < mItems.size(), "Invalid index in selections.");
	return mItems[index]->mGrid;
}

bool Selections::GetItemShow(const unsigned int index) const
{
	Debug::CheckAssertion(index < mItems.size(), "Invalid index in selections.");
	return mItems[index]->mShow;
}

bool Selections::IsSelected(const unsigned int index) const
{
	Debug::CheckAssertion(index < mItems.size(), "Invalid index in selections.");
	
	return mItems[index]->mSelected;
}

Widget * Selections::Find(string & id, const bool activited)
{
	return nullptr;
}

const Widget * Selections::Find(string & id, const bool activied) const
{
	return nullptr;
}

Widget * Selections::FindAt(const Point2 & pos)
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	Widget* widget = mPlacement->FindAt(pos);
	if (widget)
	{
		return widget;
	}

	if (Widget::FindAt(pos))
	{
		return this;
	}
	return nullptr;
}

void Selections::Place(const Point2 & pos, const Size & size)
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	Widget::Place(pos,size);	

	mPlacement->Place(pos, size);
}

Size Selections::CalculateBestSize() const
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	return mPlacement->CalculateBestSize();
}

void Selections::DoSelectedItem(unsigned int index)
{
	Debug::CheckAssertion(index < mItems.size(), "Invalid index in selections.");

	mSelectedItemCount++;
	SetItemSelected(index, true);
}

void Selections::DoDeSelectedItem(unsigned int index)
{
	Debug::CheckAssertion(index < mItems.size(), "Invalid index in selections.");

	mSelectedItemCount--;
	SetItemSelected(index, false);
}

/**
*	\brief 设置item的selected状态
*
*	由doSelectItem和doDeSelectedItem调用，该接口调用selecteAction组件
*/
void Selections::SetItemSelected(const unsigned int index, bool selected)
{
	Debug::CheckAssertion(mSelectionAction != nullptr, "The Selection Action is empty.");

	mItems[index]->mSelected = selected;
	auto&grid = mItems[index]->mGrid;
	
	mSelectionAction->SelectItem(grid, selected);
}

/**
*	\brief 绘制子节点
*/
void Selections::ImplDrawChildren(const Point2 & offset)
{
	for (const auto& item : mItems)
	{
		Child* it = item.get();
		if (it->mShow && it->mGrid.GetVisibility() == Visiblility::Visible)
		{
			it->mGrid.DrawChildren(offset);
		}
	}
}

void Selections::SignalHanderKeyDown(const ui_event event, bool & handle, const InputEvent::KeyboardKey key)
{
	switch (key)
	{
	case InputEvent::KEY_LEFT:
		HandlerKeyLeft(handle);
		break;
	case InputEvent::KEY_RIGHT:
		HandlerKeyRight(handle);
		break;
	case InputEvent::KEY_UP:
		HandlerKeyUp(handle);
		break;
	case InputEvent::KEY_DOWN:
		HandlerKeyDown(handle);
		break;
	default:
		break;
	}
}

/**
*	\brief 当选项被鼠标事件点击时触发
*
*	当执行完后，本身也需要触发notifyMOdified事件
*/
void Selections::SignalHandlerNotifyModified(Dispatcher& dispatcher)
{
	auto widget = dynamic_cast<Widget*>(&dispatcher);
	if (widget == nullptr)
	{
		return;
	}

	for (int i = 0; i < GetItemCount(); i++)
	{
		auto& grid = mItems[i]->mGrid;
		if (grid.HasWidget(*widget))
		{
			auto toggle = dynamic_cast<ToggleButton*>(&dispatcher);
			if (toggle != nullptr)
			{
				SelectItem(i, toggle->IsSelected());
			}
			else
			{
				SelectItem(i, true);
			}
			
			Fire(ui_event::UI_EVENT_NOTIFY_MODIFIED, *this, nullptr);
			return;
		}
	}
}

void Selections::HandlerKeyLeft(bool & handle)
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	mPlacement->HandlerKeyLeft(handle);
}

void Selections::HandlerKeyRight(bool & handle)
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	mPlacement->HandlerKeyRight(handle);
}

void Selections::HandlerKeyUp(bool & handle)
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	mPlacement->HandlerKeyUp(handle);
}

void Selections::HandlerKeyDown(bool & handle)
{
	Debug::CheckAssertion(mPlacement != nullptr, "The Placement is empty.");
	mPlacement->HandlerKeyDown(handle);
}

/******* ******* ******* Placement  ******* ******** ************/


void HorizontalList::HandlerKeyLeft(bool& handle)
{
	if (mSelections->GetItemCount() == 0)
	{
		return;
	}
	// 如果为选择任何选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = mSelections->GetItemCount() - 1; i >= 0; i--)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}

		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() - 1; i >= 0; i--)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

void HorizontalList::HandlerKeyRight(bool& handle)
{
	if (mSelections->GetItemCount() == 0)
	{
		return;
	}

	// 如果未存在已选择选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}
		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() + 1; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

/**
*	\brief 水平辅助辅助放置函数
*/
void HorizontalList::Place(const Point2& pos, const Size& size)
{
	// 水平排列放置各个item
	Point2 curOrigin = pos;
	for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i) )
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		Size bestSize = grid.GetBestSize();
		bestSize.height = size.height;
		grid.Place(curOrigin, bestSize);

		curOrigin.x += bestSize.width;
	}
}

Widget * HorizontalList::FindAt(const Point2 & pos)
{
	for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		Widget* widget = grid.FindAt(pos);

		if (widget)
		{
			return widget;
		}
	}

	return nullptr;
}

Size HorizontalList::CalculateBestSize() const
{
	Size result(0, 0);
	for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		const Size bestSize = grid.GetBestSize();

		if (bestSize.height > result.height)
		{
			result.height = bestSize.height;
		}

		result.width += bestSize.width;
	}
	return result;
}

void VerticalList::HandlerKeyUp(bool& handle)
{
	if (mSelections->GetItemCount() == 0)
	{
		return;
	}
	// 如果为选择任何选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = mSelections->GetItemCount() - 1; i >= 0; i--)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}

		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() - 1; i >= 0; i--)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

void VerticalList::HandlerKeyDown(bool & handle)
{
	if (mSelections->GetItemCount() == 0)
	{
		return;
	}

	// 如果未存在已选择选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}
		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() + 1; i < mSelections->GetItemCount(); i++)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

/**
*	\brief 垂直辅助辅助放置函数
*/
void VerticalList::Place(const Point2 & pos, const Size & size)
{
	// 垂直排列放置各个item
	Point2 curOrigin = pos;
	for (int i = 0; i < mSelections->GetItemCount(); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		Size bestSize = grid.GetBestSize();
		bestSize.width = size.width;
		grid.Place(curOrigin, bestSize);

		curOrigin.y += bestSize.height;
	}
}

Widget * VerticalList::FindAt(const Point2 & pos)
{
	for (int i = 0; i < mSelections->GetItemCount(); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		Widget* widget = grid.FindAt(pos);

		if (widget)
		{
			return widget;
		}
	}

	return nullptr;
}

Size VerticalList::CalculateBestSize() const
{
	Size result(0, 0);
	for (int i = 0; i < mSelections->GetItemCount(); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		const Size bestSize = grid.GetBestSize();

		if (bestSize.width > result.width)
		{
			result.width = bestSize.width;
		}

		result.height += bestSize.height;
	}
	return result;
}

/**
*	\brief 添加一个新项
*
*	目前不考虑不同享之间大小不同
*/
void TableList::AddItem(Widget & widget)
{

}

void TableList::HandlerKeyLeft(bool& handle)
{
	if (mSelections->GetItemCount() == 0)
	{
		return;
	}
	// 如果为选择任何选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = mSelections->GetItemCount() - 1; i >= 0; i--)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}

		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() - 1; i >= 0; i--)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

void TableList::HandlerKeyRight(bool & handle)
{
	if (mSelections->GetItemCount() == 0)
	{
		return;
	}

	// 如果未存在已选择选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = 0; i < mSelections->GetItemCount(); i++)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}
		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() + 1; i < mSelections->GetItemCount(); i++)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

void TableList::HandlerKeyUp(bool & handle)
{
	if (mSelections->GetItemCount() == 0 || mSelections->mItemCols == 0)
	{
		return;
	}
	// 如果为选择任何选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = mSelections->GetItemCount() - 1; i >= 0; i--)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}

		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() - mSelections->mItemCols; i >= 0; i-= mSelections->mItemCols)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

void TableList::HandlerKeyDown(bool & handle)
{
	if (mSelections->GetItemCount() == 0 || mSelections->mItemCols == 0)
	{
		return;
	}

	// 如果未存在已选择选择项
	if (mSelections->GetSelectedItemCount() == 0)
	{
		for (int i = 0; i < mSelections->GetItemCount(); i++)
		{
			if (mSelections->GetItemShow(i))
			{
				handle = true;
				mSelections->SelectItem(i, true);
				break;
			}
		}
		return;
	}
	handle = true;
	// 像做寻找可以选择项
	for (int i = mSelections->GetSelectedItem() + mSelections->mItemCols; i < static_cast<int>(mSelections->GetItemCount()); i+= mSelections->mItemCols)
	{
		if (!mSelections->GetItemShow(i))
			continue;

		auto& grid = mSelections->GetItemGrid(i);
		auto widget = std::dynamic_pointer_cast<StyledWidget>(grid.GetWidget(0, 0));

		if (widget && widget->GetActivite())
		{
			mSelections->SelectItem(i, true);
			return;
		}
	}
}

void TableList::Place(const Point2 & pos, const Size & size)
{
	// 垂直排列放置各个item
	Point2 curOrigin = pos;
	int tmpCols = 0;

	mSelections->mItemCols = 0;
	mSelections->mItemRows = 1;
	
	for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		Size bestSize = grid.GetBestSize();

		if (curOrigin.x + bestSize.width > pos.x + size.width)
		{
			curOrigin.x = pos.x;
			curOrigin.y += bestSize.height;

			mSelections->mItemRows++;
			if (tmpCols > mSelections->mItemCols)
			{
				mSelections->mItemCols = tmpCols;
			}

			tmpCols = 0;
		}

		grid.Place(curOrigin, bestSize);
		curOrigin.x += bestSize.width;
		tmpCols++;
	}
	if (tmpCols > mSelections->mItemCols)
	{
		mSelections->mItemCols = tmpCols;
	}
}

Widget * TableList::FindAt(const Point2 & pos)
{
	for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		Widget* widget = grid.FindAt(pos);

		if (widget)
		{
			return widget;
		}
	}

	return nullptr;
}

/**
*	\brief 计算最佳大小
*
*	目前不考虑各个item不同大小对其他因素的影响
*/
Size TableList::CalculateBestSize() const
{


	Size result(0, 0);
	for (int i = 0; i < static_cast<int>(mSelections->GetItemCount()); i++)
	{
		if (!mSelections->GetItemShow(i))
		{
			continue;
		}
		auto& grid = mSelections->GetItemGrid(i);
		const Size bestSize = grid.GetBestSize();

		if (bestSize.height > result.height)
		{
			result.height = bestSize.height;
		}

		result.width += bestSize.width;
	}
	return result;
}

/******* ******* ******* Selection Action  ******* ******** ************/

void Selected::InitItem(Widget* widget)
{
	if (widget && widget->GetControlType() == "ToggleButton")
	{
		auto modifiedCallback = std::function<void(Dispatcher& dispatcher)>(
			std::bind(&Selections::SignalHandlerNotifyModified, mSelections, std::placeholders::_1));
		
		widget->ConnectSignal<ui_event::UI_EVENT_NOTIFY_MODIFIED>(std::bind(modifiedCallback, std::placeholders::_1));
	}
}

void Selected::SelectItem(SingleGrid& grid, bool selected )
{
	auto widget = std::dynamic_pointer_cast<SelectableItem>(grid.GetWidget(0, 0));
	Debug::CheckAssertion(widget != nullptr,
		"The Selectable thing is empty in select item.");

	widget->SetSelected(selected);
}

/******* ******* ******* Minmum Selection  ******* ******** ************/



}