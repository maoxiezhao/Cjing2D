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
	// ֪ͨ����widget,��widget���
	Widget* parent = GetParent();
	while (parent)
	{
		parent->Fire(gui::UI_EVENT_NOTIFY_REMOVE, *parent);
		parent = parent->GetParent();
	}

	// link group ???
	if (!mLinkedGroup.empty())
	{

	}
}

/**
*	\brief ����widget ID
*	\param id Ψһidֵ
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
*	\brief ��ȡ�㼶����
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

const string Widget::GetControlType() const
{
	return "Widget";
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

const Point2 & Widget::GetWantedPositon() const
{
	return mWantedPosition;
}

void Widget::SetWantedPosition(const Point2 & position)
{
	mWantedPosition = position;
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

void Widget::RefreshPlace()
{
}

/**
*	\brief λ�Ƶ�ǰwidget
*	\param offset ƫ����
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
*	\brief ��ʼ������
*/
void Widget::InitLayout()
{
	Debug::CheckAssertion(mVisible != Visiblility::InVisible);
	Debug::CheckAssertion(GetWindow() != nullptr);

	mLayoutSize = Size(0, 0);
}

/**
*	\brief ������Ѵ�С
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
*	\brief ������Ѵ�С
*
*	����ʵ����������ʵ��
*/
Size Widget::CalculateBestSize()const
{
	// nothing.PS ����DEBUG���ǣ�����size
	return mSize;
}

/**
*	\brief ���¼�����Ѵ�С
*
*	����ʵ����������ʵ��
*/
Size Widget::ReCalculateBestSize() 
{
	return Size();
}


/**
*	\brief ����ˮƽ����
*/
void Widget::SetHorizontalAlignment(const unsigned int align)
{
	Grid* parentGrid = GetParentGrid();
	if (parentGrid == nullptr)
	{
		return;
	}
	parentGrid->SetChildrenAlignment(std::dynamic_pointer_cast<Widget>(this->shared_from_this()), 
			align, gui::ALIGN_VERTICAL_MASK);
}

/**
*	\brief ���ô�ֱ����
*/
void Widget::SetVerticalAlignment(const unsigned int align)
{
	Grid* parentGrid = GetParentGrid();
	if (parentGrid == nullptr)
	{
		return;
	}
	parentGrid->SetChildrenAlignment(std::dynamic_pointer_cast<Widget>(this->shared_from_this()),
		align, gui::ALIGN_VERTICAL_MASK);
}

bool Widget::CanWrap() const
{
	return false;
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

void Widget::Draw()
{
	DrawBackground();
	DrawChildren();
	DrawForeground();
}

/**
*	\brief ���Ʊ���ͼ
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
		//DrawDebugGround();
		ImplDrawBackground(offset);
	}
}

/**
*	\brief ����ǰ��ͼ
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
*	\bref ����children
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
*	\brief ��debugģʽ�»��������������
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

const string Widget::GetLuaObjectName() const
{
	return string();
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
*	\brief ����widget�Ŀɼ���
*
*	������widget�ӿɼ���Ϊ���ɼ���Ӳ��ɼ���Ϊ�ɼ�ʱ
*	��Ҫ���µ�����С
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
	return this == &widget;
}

bool Widget::IsAt(const Point2& pos)const
{
	return  pos.x >= mPosition.x &&
			pos.y >= mPosition.y &&
			pos.x <= (mPosition.x + mSize.width) &&
			pos.y <= (mPosition.y + mSize.height);
}

Widget * Widget::FindAt(const Point2 & pos)
{
	return IsAt(pos) ? this : nullptr;
}


}