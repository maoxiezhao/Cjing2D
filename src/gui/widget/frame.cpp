#include "gui\widget\frame.h"
#include "gui\widget\widget.h"
#include "gui\lua\uiApi.h"

namespace gui
{

/***** ****** ***** window manager ***** ****** *****/

class Manager
{
	Manager();
public:
	static Manager& GetInstance();
	
	void Add(Frame& window);
	void Remove(Frame& window);
	unsigned int GetID(Frame& window);
	Frame* GetFrame(const unsigned int id);

private:
	std::map<unsigned int, Frame*> mFrames;

};

Manager::Manager() :mFrames()
{
}

Manager & Manager::GetInstance()
{
	static Manager windowManager;
	return windowManager;
}

void Manager::Add(Frame & window)
{
	static unsigned int id;
	++id;
	mFrames[id] = &window;
}

void Manager::Remove(Frame & window)
{
	for (auto it = mFrames.begin(); it != mFrames.end(); ++it)
	{
		if (it->second == &window)
		{
			mFrames.erase(it);
			return;
		}
	}
}

unsigned int Manager::GetID(Frame & window)
{
	for (auto it = mFrames.begin(); it != mFrames.end(); ++it)
	{
		if (it->second == &window)
		{
			return it->first;
		}
	}

	return 0;
}

Frame * Manager::GetFrame(const unsigned int id)
{
	auto it = mFrames.find(id);
	if (it != mFrames.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

/***** ****** ***** ***** window ***** ***** ****** *****/

Frame::Frame()
{
}

Frame::Frame(int x, int y, int w, int h) :
	ContainerBase(),
	mPosX(x),
	mPosY(y),
	mWidth(w),
	mHeight(h),
	mSuspendDrawing(false),
	mNeedLayout(true),
	mDistributor(nullptr)
{
	Manager::GetInstance().Add(*this);
	Connect();
	RegisterSignalCallBack();

	Place({ x, y }, { w, h });
}

Frame::~Frame()
{
	Manager::GetInstance().Remove(*this);
}

/**
*	\brief 显示窗体
*/
void Frame::Show(bool showed)
{
	mSuspendDrawing = !showed;
}

/**
*	\brief 绘制窗体
*/
void Frame::Draw()
{
	if (mSuspendDrawing)
		return;
	
	// 是否需要重新布局
	if (mNeedLayout)
	{
		LayoutChildren(GetPositon());
	}
	else
	{
		LayoutChildren(GetPositon());
	}

	// background
	DrawBackground(Point2(0, 0));
	// children
	DrawChildren(Point2(0, 0));
	// foreground
	DrawForeground(Point2(0, 0));
}

void Frame::UnDraw()
{
}

/**
*	\brief 布局操作
*/
void Frame::Layout()
{
}

const string Frame::GetControlType() const
{
	return "Frame";
}

void Frame::SetDistributor(std::shared_ptr<Distributor> distributor)
{
	mDistributor = distributor;
}

Widget * Frame::FindAt(const Point2 & pos)
{
	auto findIt = ContainerBase::FindAt(pos);
	if (findIt)
		return findIt;

	return  Widget::IsAt(pos) ? this : nullptr;
}

/**
*	\brief 注册窗体事件回调
*/
void Frame::RegisterSignalCallBack()
{
	ConnectSignal<ui_event::UI_EVENT_MOUSE_ENTER>(
		std::bind(&Frame::SignalHandlerMouseEnter, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEAVE>(
		std::bind(&Frame::SignalHandlerMouseLeave, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOWN>(
		std::bind(&Frame::SignalHandlerMouseLeftButtonDown, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_UP>(
		std::bind(&Frame::SignalHandlerMouseLeftButtonUp, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_CLICK>(
		std::bind(&Frame::SignalHandlerMouseLeftButtonClick, this, std::placeholders::_2, std::placeholders::_3));
}

void Frame::SignalHandlerMouseEnter(const ui_event event, bool & handle)
{
	handle = DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_ENTER);
}

void Frame::SignalHandlerMouseLeave(const ui_event event, bool & handle)
{
	handle = DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_LEAVE);
}

void Frame::SignalHandlerMouseLeftButtonDown(const ui_event event, bool & handle)
{
	handle = DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_DOWN);
}

void Frame::SignalHandlerMouseLeftButtonUp(const ui_event event, bool & handle)
{
	handle = DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_UP);
}

void Frame::SignalHandlerMouseLeftButtonClick(const ui_event event, bool & handle)
{
	handle = DoLuaCallBack(WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_CLICK);
}

/**
*	\brief 捕获鼠标事件
*/
void Frame::MouseCaptrue(bool mouseCpatured)
{
	Debug::CheckAssertion(mDistributor != nullptr, "The Distributor is nullptr.");
	mDistributor->MouseCaptrue(mouseCpatured);
}

void Frame::AddToKeyboardFocusChain(Widget * widget)
{
	Debug::CheckAssertion(mDistributor != nullptr, "The Distributor is nullptr.");
	mDistributor->AddToKeyBoardFocusChain(widget);
}

Size Frame::GetBestSize() const
{
	return Size(mWidth, mHeight);
}

void Frame::ClearLuaCallBack()
{
	Widget::ClearLuaCallBack();
	ContainerBase::ClearLuaCallBack();
}

const string Frame::GetLuaObjectName() const
{
	return ui_lua_frame_name;
}



}