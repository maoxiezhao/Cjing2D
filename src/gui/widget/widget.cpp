#include "widget.h"
#include "gui\widget\window.h"
#include "gui\widget\styledwidget.h"
#include "gui\widget\grid.h"

#include "core\random.h"

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
	mSize(),
	mLayoutSize()
{
	mDebugSprite = std::make_shared<Sprite>(Color4B(rand()%(255), rand() % (255), rand() % (255),255), Size(0, 0));
}

Widget::Widget(const BuilderWidget & builder):
	mID(builder.mID),
	mParent(nullptr),
	mIsDirty(true),
	mVisible(Visiblility::Visible),
	mReDrawAction(ReDrawAction::Full),
	mLinkedGroup(""),
	mPosition(),
	mSize(),
	mLayoutSize()
{
	mDebugSprite = std::make_shared<Sprite>(Color4B::YELLOW, Size(0, 0));
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

Grid * Widget::GetParentGrid()
{
	Widget* result = mParent;
	while (result && dynamic_cast<Grid*>(result) == nullptr)
	{
		result = result->GetParent();
	}
	return result ? dynamic_cast<Grid*>(result) : nullptr;
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
	Debug::CheckAssertion(size.width >= 0);
	Debug::CheckAssertion(size.height >= 0);

	mSize = size;
	SetIsDirty(true);
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
	Debug::CheckAssertion(size.width >= 0);
	Debug::CheckAssertion(size.height >= 0);

	mPosition = pos;
	mSize = size;
	SetIsDirty(true);
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
	Debug::CheckAssertion(mVisible != Visiblility::InVisible);
	Debug::CheckAssertion(GetWindow() != nullptr);

	mLayoutSize = Size(0, 0);
}

/**
*	\brief 返回最佳大小
*/
Size Widget::GetBestSize() const
{
	Debug::CheckAssertion(mVisible != Visiblility::InVisible);

	Size result = mLayoutSize;
	if (result == Size(0,0))
	{
		result = CalculateBestSize();
	}
	return result;
}

/**
*	\brief 计算最佳大小
*
*	具体实现由派生类实现
*/
Size Widget::CalculateBestSize()const
{
	// nothing.PS 处于DEBUG考虑，返回size
	return mSize;
}

/**
*	\brief 重新计算最佳大小
*
*	具体实现由派生类实现
*/
Size Widget::ReCalculateBestSize() 
{
	return Size();
}


/**
*	\brief 设置水平对齐
*/
void Widget::SetHorizontalAlignment(const unsigned int align)
{
	Grid* parentGrid = GetParentGrid();
	if (parentGrid == nullptr)
	{
		return;
	}
	parentGrid->SetChildrenAlignment(this->shared_from_this(), align, gui::ALIGN_VERTICAL_MASK);
}

/**
*	\brief 设置垂直对齐
*/
void Widget::SetVerticalAlignment(const unsigned int align)
{
	Grid* parentGrid = GetParentGrid();
	if (parentGrid == nullptr)
	{
		return;
	}
	parentGrid->SetChildrenAlignment(this->shared_from_this(), align, gui::ALIGN_VERTICAL_MASK);
}

void Widget::RequestReduceWidth(const int maxnumWidth)
{
	// nothing
}

void Widget::DemandReduceWidth(const int maxnumWidth)
{
	// nothing
}

void Widget::RequestReduceHeight(const int maxnumHeight)
{
	// nothing
}

void Widget::DemandReduceHeight(const int maxnumHeight)
{
	// nothing
}

void Widget::DrawBackground()
{
	Debug::CheckAssertion(mVisible == Visiblility::Visible);
	DrawBackground(Point2(0, 0));
}

void Widget::DrawForeground()
{
	Debug::CheckAssertion(mVisible == Visiblility::Visible);
	DrawForeground(Point2(0, 0));
}

/**
*	\brief 绘制背景图
*/
void Widget::DrawBackground(const Point2& offset)
{
	Debug::CheckAssertion(mVisible == Visiblility::Visible);

	if (mReDrawAction == ReDrawAction::Full)
	{
		DrawDebugGround();
		ImplDrawBackground(offset);
	}
	else if (mReDrawAction == ReDrawAction::Partly)
	{
		DrawDebugGround();
		ImplDrawBackground(offset);
	}
}

/**
*	\brief 绘制前景图
*/
void Widget::DrawForeground(const Point2& offset)
{
	Debug::CheckAssertion(mVisible == Visiblility::Visible);

	if (mReDrawAction == ReDrawAction::Full)
	{
		ImplDrawForeground(offset);
	}
	else if (mReDrawAction == ReDrawAction::Partly)
	{
		ImplDrawForeground(offset);
	}
}

/**
*	\bref 绘制children
*/
void Widget::DrawChildren(const Point2& offset)
{
	Debug::CheckAssertion(mVisible == Visiblility::Visible);

	if (mReDrawAction == ReDrawAction::Full)
	{
		ImplDrawChildren(offset);
	}
	else if (mReDrawAction == ReDrawAction::Partly)
	{
		ImplDrawChildren(offset);
	}
}

/**
*	\brief 在debug模式下绘制轮廓或者填充
*/
void Widget::DrawDebugGround()
{
	if (mDebugSprite != nullptr)
	{
		mDebugSprite->SetPos(mPosition);
		mDebugSprite->SetSize(mSize);
		mDebugSprite->Draw();
	}
}

void Widget::ImplDrawBackground(const Point2& offset)
{
}

void Widget::ImplDrawForeground(const Point2& offset)
{
}

void Widget::ImplDrawChildren(const Point2& offset)
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

/**
*	\brief 设置widget的可见性
*
*	当设置widget从可见变为不可见或从不可见变为可见时
*	需要重新调整大小
*/
void Widget::SetVisibility(const Visiblility & visibility)
{
	if (mVisible == visibility)
	{
		return;
	}
	bool needResize = (mVisible == Visiblility::InVisible) ||
				(visibility == Visiblility::InVisible && GetSize() == Size(0,0));
	mVisible = visibility;

	if (needResize)
	{
		if (mVisible == Visiblility::Visible)
		{
			gui::Message message;
			Fire(gui::EVENT_REQUEST_PLACEMENT, *this, message);
		}
	}
	else
	{
		SetIsDirty(true);
	}
}

void Widget::SetParent(Widget * parent)
{
	mParent = parent;
}

void Widget::SetLinkGrounp(const std::string & linkedGroup)
{
	mLinkedGroup = linkedGroup;
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
	return  pos.x >= mPosition.x &&
			pos.y >= mPosition.y &&
			pos.y <= (mPosition.x + mSize.width) &&
			pos.y <= (mPosition.y + mSize.height);
}


}