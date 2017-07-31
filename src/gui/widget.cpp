#include "widget.h"

Widget::Widget():
	mID(0),
	mParent(),
	mIsDirty(false),
	mVisible(Visiblility::Visible),
	mPosition(),
	mSize()
{
}

Widget::~Widget()
{
}

const Point2 & Widget::GetPositon() const
{
	return mPosition;
}

void Widget::SetPosition(const Point2 & position)
{
	mPosition = position;
}

const Size & Widget::GetSize() const
{
	return mSize;
}

void Widget::SetSize(const Size & size)
{
	mSize = size;
}

int Widget::GetWidth() const
{
	return mSize.width;
}

int Widget::GetHeight() const
{
	return mSize.height;
}

/**
*	\brief 位移当前widget
*	\param offset 偏移量
*/
void Widget::Move(const Point2 & offset)
{
}

void Widget::Move(const int xoffset, const int yoffset)
{
}

/**
*	\brief 初始化布局
*/
void Widget::InitLayout()
{

}

Size Widget::GetBestSize() const
{
	return Size();
}

/**
*	\brief 设置水平对齐
*/
void Widget::SetHorizontalAlignment()
{
}

/**
*	\brief 设置垂直对齐
*/
void Widget::SetVerticalAlignment()
{
}

/**
*	\brief 设置widget ID
*	\param id 唯一id值
*/
void Widget::SetID(const string & id)
{
}

const string & Widget::GetID() const
{
	// TODO: 在此处插入 return 语句
}

/**
*	\brief 绘制背景图
*/
void Widget::DrawBackground()
{
}

/**
*	\brief 绘制前景图
*/
void Widget::DrawForeground()
{
}

/**
*	\bref 绘制children
*/
void Widget::DrawChildren()
{
}

void Widget::SetIsDirty(bool isDirty)
{
	mIsDirty = isDirty;
}

bool Widget::IsDirty() const
{
	return mIsDirty;
}

const Rect Widget::GetDirtyRect() const
{
	return Rect();
}

Widget::Visiblility Widget::GetVisibility() const
{
	return mVisible;
}

void Widget::SetVisibility(const Visiblility & visibility)
{
	mVisible = visibility;
}
