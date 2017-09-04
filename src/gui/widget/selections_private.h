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
class OneItem : public MaxmunSelection
{
public:
	virtual void SelectItem(const unsigned int index, bool selected = true)
	{
		Debug::CheckAssertion(mSelections != nullptr, "The selections is empty.");

		if (mSelections->GetSelectedItemCount() == 1)
		{
			mSelections->DoSelectedItem(mSelections->GetSelectedItem());
		}
		mSelections->DoSelectedItem(index);
	}
};


/////////////////////////////////////////////////////////////////////////////

}


