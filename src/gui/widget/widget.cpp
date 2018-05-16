#include "widget.h"
#include "gui\widget\frame.h"
#include "gui\widget\styledwidget.h"
#include "gui\widget\multGrid.h"
#include "gui\lua\uiApi.h"
#include "core\random.h"
#include "movements\movement.h"

namespace gui
{

bool Widget::mDebugDraw = false;

Widget::Widget() :
	mID(""),
	mParent(nullptr),
	mIsDirty(true),
	mVisible(true),
	mReDrawAction(ReDrawAction::Full),
	mLinkedGroup(""),
	mPosition(),
	mSize(),
	mLayoutSize(),
	mGridPos(-1, -1)
{
	mDebugSprite = std::make_shared<Sprite>(Color4B(rand()%(255), rand() % (255), rand() % (255),255), Size(0, 0));
}

Widget::Widget(const BuilderWidget & builder):
	mID(builder.mID),
	mParent(nullptr),
	mIsDirty(true),
	mVisible(true),
	mReDrawAction(ReDrawAction::Full),
	mLinkedGroup(""),
	mPosition(),
	mSize(),
	mLayoutSize(),
	mGridPos(-1, -1)
{
	mDebugSprite = std::make_shared<Sprite>(Color4B::YELLOW, Size(0, 0));
}

Widget::~Widget()
{
	// 通知父类widget,该widget清除
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
*	\brief 获取父节点Frame
*/
Frame * Widget::GetParentFrame()
{
	Widget* result = this->GetParent();

	while (result != nullptr && 
		result->GetWidgetType() != WIDGET_TYPE::WIDGET_FRAME )	{
		result = result->GetParent();
	}

	if(result != nullptr)
		return dynamic_cast<Frame*>(result);

	return nullptr;
}

/**
*	\brief 获取层级窗体
*/
Frame * Widget::GetRoot()
{
	Widget* result = this;
	while (result->GetParent() != nullptr)
	{
		result = result->GetParent();
	}
	return dynamic_cast<Frame*>(result);
}

const Frame * Widget::GetRoot() const
{
	const Widget* result = this;
	while (result->mParent != nullptr)
	{
		result = result->mParent;
	}
	return dynamic_cast<const Frame*>(result);
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
	Debug::CheckAssertion(GetRoot() != nullptr);
	mLayoutSize = Size(0, 0);
}

/**
*	\brief 返回最佳大小
*/
Size Widget::GetBestSize() const
{
	Debug::CheckAssertion(mVisible, 
		"Try to get best size for invisable wiget.");

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
	parentGrid->SetChildrenAlignment(std::dynamic_pointer_cast<Widget>(this->shared_from_this()), 
			align, gui::ALIGN_VERTICAL_MASK);
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
	parentGrid->SetChildrenAlignment(std::dynamic_pointer_cast<Widget>(this->shared_from_this()),
		align, gui::ALIGN_VERTICAL_MASK);
}

/**
*	\brief 设置该网格位置，仅用来记录作用
*/
void Widget::SetGridPos(const Point2 & pos)
{
	mGridPos = pos;
}

Point2 Widget::GetGridPos() const
{
	return mGridPos;
}

/**
*	\brief 将该Widget从父Frame移到最顶上
*/
void Widget::ToTopByParent()
{
	Frame* parent = GetParentFrame();
	if (parent != nullptr)
	{
		Point2 gridPos = GetGridPos();
		if(gridPos.x >= 0 && gridPos.y >= 0)
			parent->TopChildren(*this, gridPos.x, gridPos.y);
	}
}

bool Widget::CanWrap() const
{
	return false;
}

bool Widget::CanMouseFocus()
{
	return Dispatcher::HasSignal<gui::setEventMouse>(event_queue_type::child);
}

void Widget::SetDebugDraw(bool draw)
{
	mDebugDraw = draw;
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

void Widget::Update()
{
	if (mMovement != nullptr)
	{
		mMovement->Update();
		if (mMovement != nullptr && mMovement->IsFinished())
			StopMovement();
	}
}

void Widget::Draw()
{
	DrawBackground();
	DrawChildren();
	DrawForeground();
}

/**
*	\brief 绘制背景图
*/
void Widget::DrawBackground(const Point2& offset)
{
	Debug::CheckAssertion(mVisible, "Try to draw in-vaisible widget");

	if (mReDrawAction == ReDrawAction::Full)
	{
		if(mDebugDraw)
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
*	\brief 绘制前景图
*/
void Widget::DrawForeground(const Point2& offset)
{
	Debug::CheckAssertion(mVisible, "Try to draw in-vaisible widget");

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
	Debug::CheckAssertion(mVisible, "Try to draw in-vaisible widget");

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

/**
*	\brief 设置事件的Lua回调
*/
void Widget::SetLuaCallBack(WIDGET_CALL_BACK_TYPE type, const LuaRef & callback)
{
	if (!mCallbacks.HasCallBack(type))
		AddLuaCallbackSignal(type);

	mCallbacks.AddCallBack(type, callback);
}

void Widget::AddLuaCallbackSignal(WIDGET_CALL_BACK_TYPE type)
{
	switch (type)
	{
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_ENTER:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_ENTER>(
			std::bind(&Widget::SignalHandlerMouseEnter, this, std::placeholders::_2, std::placeholders::_3));
		break;
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_LEAVE:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_LEAVE>(
			std::bind(&Widget::SignalHandlerMouseLeave, this, std::placeholders::_2, std::placeholders::_3));
		break;
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_DOWN:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOWN>(
			std::bind(&Widget::SignalHandlerMouseLeftButtonDown, this, std::placeholders::_2, std::placeholders::_3));
		break;
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_UP:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_UP>(
			std::bind(&Widget::SignalHandlerMouseLeftButtonUp, this, std::placeholders::_2, std::placeholders::_3));
		break;
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_CLICK:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_CLICK>(
			std::bind(&Widget::SignalHandlerMouseLeftButtonClick, this, std::placeholders::_2, std::placeholders::_3));
		break;
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_DOUBLE_CLICK:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOUBLE_CLICK>(
			std::bind(&Widget::SignalHandlerMouseLeftButtonDoubleClick, this, std::placeholders::_2, std::placeholders::_3));
		break;
	case WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_HOVER:
		ConnectSignal<ui_event::UI_EVENT_MOUSE_MOTION>(
			std::bind(&Widget::SignalHandlerMouseHover, this, std::placeholders::_2, std::placeholders::_3));
		break;
	}
}

void Widget::ClearLuaCallBack()
{
	mCallbacks.Clear();
}

bool Widget::DoLuaCallBack(WIDGET_CALL_BACK_TYPE type)
{
	return mCallbacks.DoCallBack(type, *this);
}

void Widget::StopMovement()
{
	mMovement = nullptr;
}

void Widget::StartMovement(const std::shared_ptr<Movement>& movement)
{
	StopMovement();
	mMovement = movement;
	mMovement->SetWidget(this);
	mMovement->SetSuspended(false);
	mMovement->Start();
}

const std::shared_ptr<Movement>& Widget::GetMovement()
{
	return mMovement;
}

void Widget::SignalHandlerMouseEnter(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_ENTER);
}

void Widget::SignalHandlerMouseLeave(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_LEAVE);
}

void Widget::SignalHandlerMouseHover(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_HOVER);
}

void Widget::SignalHandlerMouseLeftButtonDown(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_DOWN);
}

void Widget::SignalHandlerMouseLeftButtonUp(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_UP);
}

void Widget::SignalHandlerMouseLeftButtonClick(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_CLICK);
}

void Widget::SignalHandlerMouseLeftButtonDoubleClick(const ui_event event, bool & handle)
{
	DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_DOUBLE_CLICK);
}

const string Widget::GetLuaObjectName() const
{
	return ui_lua_widget_name;
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

bool Widget::GetVisibility() const
{
	return mVisible;
}

/**
*	\brief 设置widget的可见性
*
*	当设置widget从可见变为不可见或从不可见变为可见时
*	需要重新调整大小
*/
void Widget::SetVisibility(const bool & visibility)
{
	if (mVisible == visibility)
	{
		return;
	}
	bool needResize = !(GetSize() == Size(0,0));
	mVisible = visibility;

	if (needResize)
	{
		gui::Message message;
		Fire(gui::EVENT_REQUEST_PLACEMENT, *this, message);
	}
	else
	{
		SetIsDirty(true); // to remove
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
	return  GetVisibility() && 
			pos.x >= mPosition.x &&
			pos.y >= mPosition.y &&
			pos.x <= (mPosition.x + mSize.width) &&
			pos.y <= (mPosition.y + mSize.height);
}

Widget * Widget::FindAt(const Point2 & pos)
{
	return IsAt(pos) ? this : nullptr;
}


}