#pragma once

#include"common\common.h"
#include"gui\widget\widget.h"

/**
*	\brief ´°Ìו
*/

namespace gui
{
class Widget;

class Window : public Widget
{
public:
	Window();
	~Window();

private:
	virtual void ImplDrawBackground();
	virtual void ImplDrawForeground();
	virtual void ImplDrawChildren();

public:
	virtual void SetPosition(const Point2& position);
	virtual void SetSize(const Size& size);

	virtual void Place(const Point2& pos, const Size& size);
	virtual void Move(const Point2& offset);
	virtual void Move(const int xoffset, const int yoffset);

	virtual void InitLayout();
	virtual Size GetBestSize()const;

	virtual void SetHorizontalAlignment();
	virtual void SetVerticalAlignment();

private:
	Point2 mPosition;
	Size mSize;

public:
	virtual Widget* Find(string& id, const bool activited);
	virtual const Widget* Find(string& id, const bool activied)const;
	virtual bool HasWidget(const Widget& widget)const;
	virtual bool IsAt(const Point2& pos)const;

};


}