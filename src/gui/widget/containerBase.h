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
	virtual void DemandReduceWidth(const int maxnumWidth);

	void ReduceHeight(const int maxnumHeight);
	virtual void RequestReduceHeight(const int maxnumHeight);
	virtual void DemandReduceHeight(const int maxnumHeight);

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


private:
	Grid mGrid;

};


}