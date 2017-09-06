#pragma once

#include"gui\widget\grid.h"
#include"gui\widget\selections.h"

namespace gui
{
/**
*	\brief selections 辅助类基类
*/
class SelectionHelper
{
public:
	void SetSelections(Selections* selections)
	{
		mSelections = selections;
	}
protected:
	Selections* mSelections;
};

/******* ******* ******* Placement  ******* ******** ************/

/**
*	\brief selections 布局辅助类
*/

class Placement : public SelectionHelper
{
public:
	virtual void AddItem(Widget& widget)
	{
	}

	virtual void HandlerKeyLeft(bool& handle)
	{
		// do nothing
	}
	virtual void HandlerKeyRight(bool& handle)
	{
		// do nothing
	}
	virtual void HandlerKeyUp(bool& handle)
	{
		// do nothing
	}
	virtual void HandlerKeyDown(bool& handle)
	{
		// do nothing
	}

	virtual void Place(const Point2& pos, const Size& size)
	{
		// do nothing
	}
	virtual Widget* FindAt(const Point2& pos)
	{
		return nullptr;
	}
	virtual Size CalculateBestSize()const
	{
		return Size();
	}
};

class HorizontalList : public Placement
{
public:
	virtual void HandlerKeyLeft(bool& handle);
	virtual void HandlerKeyRight(bool& handle);

	virtual void Place(const Point2& pos, const Size& size);
	virtual Widget* FindAt(const Point2& pos);
	virtual Size CalculateBestSize()const;
};

class VerticalList : public Placement
{
public:
	virtual void HandlerKeyUp(bool& handle);
	virtual void HandlerKeyDown(bool& handle);

	virtual void Place(const Point2& pos, const Size& size);
	virtual Widget* FindAt(const Point2& pos);
	virtual Size CalculateBestSize()const;
};

class TableList : public Placement
{
public:
	virtual void AddItem(Widget& widget);

	virtual void HandlerKeyLeft(bool& handle);
	virtual void HandlerKeyRight(bool& handle);
	virtual void HandlerKeyUp(bool& handle);
	virtual void HandlerKeyDown(bool& handle);

	virtual void Place(const Point2& pos, const Size& size);
	virtual Widget* FindAt(const Point2& pos);
	virtual Size CalculateBestSize()const;
};


/******* ******* ******* MaxmumSelection  ******* ******** ************/

class MaxmunSelection : public SelectionHelper
{
public:
	virtual void SelectItem(const unsigned int index, bool selected = true) = 0;
};

/**
*	\brief 最多只能选择一个
*/
class MaxmumOneItem : public MaxmunSelection
{
public:
	virtual void SelectItem(const unsigned int index, bool selected = true)
	{
		Debug::CheckAssertion(mSelections != nullptr, "The selections is empty.");

		if (mSelections->GetSelectedItemCount() == 1)
		{
			mSelections->DoDeSelectedItem(mSelections->GetSelectedItem());
		}
		mSelections->DoSelectedItem(index);
	}
};

/******* ******* ******* SelectionAction  ******* ******** ************/

class SelectAction : public SelectionHelper
{
public:
	virtual void InitItem(Widget* widget) = 0;
	virtual void SelectItem(Grid& grid, bool selected = true) = 0;
};

class Selected : public SelectAction
{
public:
	virtual void InitItem(Widget* widget);
	virtual void SelectItem(Grid& grid, bool selected = true);
};


/******* ******* ******* MinmumSelection  ******* ******** ************/

class MinmumSelection : public SelectionHelper
{
public:
	/**
	*	\brief 关闭某个选择项的选择状态
	*	\return 如果关闭成功则返回true
	*		    反之返回false（可能需要在返回false后做一些操作）
	*/
	virtual bool DeSelectItem(const unsigned int index) = 0;
};

/**
*	\brief 最少只能选择一个
*/
class MinmumOneItem : public MinmumSelection
{
public:
	virtual bool DeSelectItem(const unsigned int index)
	{
		if (mSelections->GetSelectedItemCount() > 1)
		{
			mSelections->DoDeSelectedItem(index);
			return true;
		}
		return false;
	}
};

}


