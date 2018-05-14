#pragma once

#include"gui\widget\widget.h"
#include"gui\widget\singleGrid.h"

namespace gui
{

class SelectionHelper;
class MaxmunSelection;
class Placement;
class SelectAction;
class MinmumSelection;
class VerticalList;

/**
*	\brief selections以特定的形式维护网格集合
*	目前支持垂直列表、水平列表、以及网格的形式
*	来维护。slections是一个辅助管理的类，真正
*   以widget显示的应该是包含selections的widget
*
*	存在下列管理辅助类：
*	placement 用于管理item的位置
*	selecAction 用于管理选择操作
*	minmumSelection 最少数量的deselection操作
*	maxSelection 最大数量的selecion操作
*/

class Selections : public Widget
{
public:
	friend class MinmumOneItem;
	friend class MaxmumOneItem;
	friend class HorizontalList;
	friend class Selected;
	friend class TableList;

	Selections();
	Selections(Placement* placement, 
		SelectAction* selectAction,
		MaxmunSelection* maxmumSelection,
		MinmumSelection* minmumSelection);

	~Selections();

	/** item manager */
	void AddItem(WidgetPtr widget);


	/** selection operator */
	void SelectItem(const unsigned int index, bool selected = true); 
	bool IsSelected(const unsigned int index)const;
	int GetItemCount()const;
	int GetSelectedItemCount()const;
	int GetSelectedItem();
	SingleGrid& GetItemGrid(const unsigned int index);
	bool GetItemShow(const unsigned int index)const;
	
	/** widget */
	virtual Widget* Find(string& id, const bool activited);
	virtual const Widget* Find(string& id, const bool activied)const;
	virtual Widget* FindAt(const Point2& pos);

	/** testing */
	virtual void Place(const Point2& pos, const Size& size);
	virtual Size CalculateBestSize()const;

protected:
	void DoSelectedItem(unsigned int index);
	void DoDeSelectedItem(unsigned int index);
	void SetItemSelected(const unsigned int index, bool selected);

private:
	int mSelectedItemCount;
	int mLastSelectedItem;
	int mItemCols;
	int mItemRows;

	/** children manager */
	struct Child 
	{
		Child():mSelected(false),mShow(true), mGrid(),mOrderedIndex(0){}

		bool mSelected;
		bool mShow;
		SingleGrid mGrid;
		int  mOrderedIndex;
	};

	std::vector<std::unique_ptr<Child> > mItems;

private:

	virtual void ImplDrawChildren(const Point2& offset);

	/** signal handler testing */
	void SignalHanderKeyDown(const ui_event event,bool& handle,const InputEvent::KeyboardKey key);
	void SignalHandlerNotifyModified(Dispatcher& dispatcher);

	void HandlerKeyLeft(bool& handle);
	void HandlerKeyRight(bool& handle);
	void HandlerKeyUp(bool& handle);
	void HandlerKeyDown(bool& handle);

	/** 辅助组件 */
	std::unique_ptr<Placement> mPlacement;
	std::unique_ptr<SelectAction> mSelectionAction;
	std::unique_ptr<MaxmunSelection> mMaxmumSelection;
	std::unique_ptr<MinmumSelection> mMinmumSelection;
};

}