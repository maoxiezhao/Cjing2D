#include "selections.h"
#include "gui\widget\selections_private.h"

namespace gui
{

Selections::Selections():
	mSelectedItemCount(0)
{
	ConnectSignal<ui_event::UI_EVENT_KEY_DOWN>(
		std::bind(&Selections::SignalHanderKeyDown, this, std::placeholders::_2, std::placeholders::_3, std::placeholders::_5));
}

Selections::Selections(Placement * placement):
	mPlacement(std::unique_ptr<Placement>(placement)),
	mSelectedItemCount(0)
{
	mPlacement->SetSelections(this);
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
	item->mOrderedIndex = mSelectedItemCount++;
	item->mGrid.SetRowCols(1, 1);
	item->mGrid.SetChildren(widget, 0, 0, ALIGN_HORIZONTAL_CENTER | ALIGN_VERTICAL_TOP, 0);
	item->mGrid.Place(Point2(0, 0), widget->GetSize());
	item->mGrid.SetParent(this);
	mItems.push_back(std::unique_ptr<Child>(item));
}

/**
*	\brief 更改某个选择项的选择状态
*/
void Selections::SelectItem(const unsigned int index, bool selected)
{
	Debug::CheckAssertion(index < GetItemCount(), "Invalid index in selecting item.");

	if (selected && !IsSelected(index))
	{	// 增加selected item
		
	}
	else if (IsSelected(index))
	{	// 减少selected item

	}
}

unsigned int Selections::GetItemCount() const
{
	return mItems.size();
}

int Selections::GetSelectedItemCount() const
{
	return 0;
}

int Selections::GetSelectedItem()
{
	return 0;
}

Grid & Selections::GetItemGrid(const unsigned int index)
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
	return false;
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
}

void Selections::DoDeSelectedItem(unsigned int index)
{
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

void Selections::HandlerKeyLeft(bool & handle)
{
}

void Selections::HandlerKeyRight(bool & handle)
{
}

void Selections::HandlerKeyUp(bool & handle)
{
}

void Selections::HandlerKeyDown(bool & handle)
{
}

/******* ******* ******* Placement  ******* ******** ************/


void HorizontalList::HandlerKeyUp(bool& handle)
{
}

void HorizontalList::HandlerKeyDown(bool& handle)
{
}

/**
*	\brief 水平辅助辅助放置函数
*/
void HorizontalList::Place(const Point2& pos, const Size& size)
{
	// 水平排列放置各个item
	Point2 curOrigin = pos;
	for (size_t i = 0; i < mSelections->GetItemCount(); i++)
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
	for (size_t i = 0; i < mSelections->GetItemCount(); i++)
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
	for (size_t i = 0; i < mSelections->GetItemCount(); i++)
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

}