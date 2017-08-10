#include "widget.h"
#include "window.h"

namespace gui
{

Widget::Widget() :
	mID(""),
	mParent(nullptr),
	mIsDirty(true),
	mVisible(Visiblility::Visible),
	mReDrawAction(ReDrawAction::Full),
	mLinkedGroup(""),
	mPosition(),
	mSize()
{
}

Widget::~Widget()
{
	// 通知父类widget,该widget清除
	Widget* parent = GetParent();
	while (parent)
	{
		parent->Fire(gui::EVENT_NOTIFY_REMOVE, *parent);
		parent = parent->GetParent();
	}

	// link group ???
	if (!mLinkedGroup.empty())
	{

	}
}

/**
*	\brief 设置widget ID
*	\param id 唯一id值
*/
void Widget::SetID(const string & id)
{
	mID = id;
}

const string & Widget::GetID() const
{
	return mID;
}

Widget * Widget::GetParent()
{
	return mParent;
}

/**
*	\brief 获取层级窗体
*/
Window * Widget::GetWindow()
{
	Widget* result = this;
	while (result->GetParent() != nullptr)
	{
		result = result->GetParent();
	}
	return dynamic_cast<Window*>(result);
}

const Window * Widget::GetWindow() const
{
	const Widget* result = this;
	while (result->mParent != nullptr)
	{
		result = result->mParent;
	}
	return dynamic_cast<const Window*>(result);
}

/***** ****** ****** Layout and size  ****** ****** *******/

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

void Widget::Place(const Point2 & pos, const Size & size)
{
	mPosition = pos;
	mSize = size;
}

/**
*	\brief 位移当前widget
*	\param offset 偏移量
*/
void Widget::Move(const Point2 & offset)
{
	mPosition += offset;
}

void Widget::Move(const int xoffset, const int yoffset)
{
	mPosition += Point2(xoffset, yoffset);
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
*	\brief 绘制背景图
*/
void Widget::DrawBackground(const Point2& offset)
{
}

/**
*	\brief 绘制前景图
*/
void Widget::DrawForeground(const Point2& offset)
{
}

/**
*	\bref 绘制children
*/
void Widget::DrawChildren(const Point2& offset)
{
}

/**
*	\brief 在debug模式下绘制轮廓或者填充
*/
void Widget::DrawDebugGround()
{
}

void Widget::ImplDrawBackground()
{
}

void Widget::ImplDrawForeground()
{
}

void Widget::ImplDrawChildren()
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

void Widget::SetParent(Widget * parent)
{
}

void Widget::SetLinkGrounp(const std::string & linkedGroup)
{
}

Widget* Widget::Find(string& id, const bool activited)
{
	return nullptr;
}

const Widget* Widget::Find(string& id, const bool activied)const
{
	return nullptr;
}

bool Widget::HasWidget(const Widget& widget)const
{
	return false;
}

bool Widget::IsAt(const Point2& pos)const
{
	return false;
}

Size Widget::CalculateBestSize()const
{
	return Size();
}

}