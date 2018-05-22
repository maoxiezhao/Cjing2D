#pragma once

#include"common\common.h"
#include"gui\widget\multGrid.h"
#include"gui\widget\styledwidget.h"

namespace gui
{
/**
*	\brief ÈÝÆ÷»ùÀà
*/
class ContainerBase : public StyledWidget
{
public:
	ContainerBase();

	virtual void InitLayout();
	virtual void Update();

	void ReduceWidth(const int maxnumWidth);
	virtual void RequestReduceWidth(const int maxnumWidth);

	void ReduceHeight(const int maxnumHeight);
	virtual void RequestReduceHeight(const int maxnumHeight);

	virtual Size GetBestSize()const;
	virtual Size CalculateBestSize()const;
	virtual bool CanWrap() const;

	virtual void Place(const Point2& pos, const Size& size);
	virtual void RefreshPlace();

	/**** ***** ***** children **** ***** *****/

	virtual void LayoutChildren(const Point2& origin);
	virtual bool HasWidget(const Widget& widget)const;
	virtual void ImplDrawChildren(const Point2& offset);

public:
	/**** ***** ***** grid **** ***** *****/

	void InitGrid();
	void AddRows(int rowCount = 1)
	{
		mGrid.AddRows(rowCount);
	}
	void AddCols(int colCount = 1)
	{
		mGrid.AddCols(colCount);
	}
	void SetCols(int cols)
	{
		mGrid.SetCols(cols);
	}
	int GetCols()const
	{
		return mGrid.GetCols();
	}
	void SetRows(int rows)
	{
		mGrid.SetRows(rows);
	}
	int GetRows()const
	{
		return mGrid.GetRows();
	}
	void SetRowCols(int row, int col)
	{
		mGrid.SetRowCols(row, col);
	}
	void SetRowFactory(const std::vector<float>& factory)
	{
		mGrid.SetRowsFactory(factory);
	}
	void SetColFactory(const std::vector<float>& factory)
	{
		mGrid.SetColsFactory(factory);
	}
	void SetChildren(const WidgetPtr& widget, int row, int col, const unsigned int flag, int borderSize)
	{
		mGrid.SetChildren(widget, row, col, flag, borderSize);
	}
	void TopChildren(Widget& widget, int row, int col)
	{
		mGrid.TopChildren(widget, row, col);
	}

	void RemoveChildrenByID(const string& id)
	{
		mGrid.RemoveChildrenByID(id);
	}

	void RemoveChildren(Widget& widget)
	{
		mGrid.RemoveChildren(widget);
	}

	void RemoveAllChildrens()
	{
		mGrid.RemoveAllChildren();
	}

	// iterator
	Grid::iterator begin()
	{
		return mGrid.begin();
	}
	Grid::iterator end()
	{
		return mGrid.end();
	}

	virtual Widget* FindAt(const Point2& pos)
	{
		return mGrid.FindAt(pos);
	}

	virtual void ClearLuaCallBack()
	{
		mGrid.ClearLuaCallBack();
	}
	
private:
	Grid mGrid;

};


}