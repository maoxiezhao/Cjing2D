#pragma once

#include"common\common.h"
#include"gui\widget\grid.h"
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

	void ReduceWidth(const int maxnumWidth);
	virtual void RequestReduceWidth(const int maxnumWidth);

	void ReduceHeight(const int maxnumHeight);
	virtual void RequestReduceHeight(const int maxnumHeight);

	virtual Size GetBestSize()const;
	virtual Size CalculateBestSize()const;
	virtual bool CanWrap() const;

	virtual void Place(const Point2& pos, const Size& size);

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
	void SetChildren(const WidgetPtr& widget, int row, int col, const unsigned int flag, int borderSize)
	{
		mGrid.SetChildren(widget, row, col, flag, borderSize);
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

private:
	Grid mGrid;

};


}