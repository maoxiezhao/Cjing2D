#include "distributor.h"
#include "gui\widget\widget.h"
#include "core\debug.h"
#include "core\system.h"

namespace gui
{

/**
*	\brief ���ڶԱ�����bool����
*/
class ResourceLocker
{
public:
	ResourceLocker(bool& locked) :mLocked(locked)
	{
		mLocked = true;
	}
	~ResourceLocker()
	{
		mLocked = false;
	}
private:
	bool &mLocked;
};

MouseMotion::MouseMotion(Widget & widget, Dispatcher::queue_position position):
	mOwner(widget),
	mMouseFocus(nullptr),
	mMouseCaptured(false),
	mSignalHandelMouseMotionEntered(false)
{
	mOwner.ConnectSignal<ui_event::UI_EVENT_MOUSE_MOTION>(
		std::bind(&MouseMotion::SignalHandleMouseMotion, this, std::placeholders::_2, std::placeholders::_3, std::placeholders::_5),position);
}

MouseMotion::~MouseMotion()
{
}

void MouseMotion::MouseCaptrue(bool captrued)
{
	mMouseCaptured = captrued;
}

void MouseMotion::MouseEnter(Widget * widget)
{
	Debug::CheckAssertion(widget);

	std::cout << ("[EVENT] Enter event. Widget ") << widget->GetID() << endl;

	mMouseFocus = widget;
	mOwner.Fire(ui_event::UI_EVENT_MOUSE_ENTER, *widget);
}

void MouseMotion::MouseLeave()
{
	std::cout << ("[EVENT] Leave event. Widget ") << mMouseFocus->GetID() << endl;

	mOwner.Fire(ui_event::UI_EVENT_MOUSE_LEAVE, *mMouseFocus);
	mMouseFocus = nullptr;
}

void MouseMotion::MouseHover(Widget * widget, const Point2 & coords)
{
	mOwner.Fire(ui_event::UI_EVENT_MOUSE_MOTION, *widget, coords);
}

/**
*	\brief ����ƶ����źŴ���
*/
void MouseMotion::SignalHandleMouseMotion(const ui_event event, bool & handle, const Point2 & coords)
{
	if (mSignalHandelMouseMotionEntered)
	{
		return;
	}
	// �Ըú�������
	ResourceLocker locker(mSignalHandelMouseMotionEntered);
	mCoords = coords;

	if (mMouseCaptured)
	{
		if (mMouseFocus)
		{
			mOwner.Fire(event, *mMouseFocus, coords);
		}
	}
	else
	{	// �ҵ����λ������widget��fire��Ӧ�¼�
		Widget* widget = mOwner.FindAt(coords);
		while (widget && !widget->CanMouseFocus() && widget->GetParent())
		{
			widget = widget->GetParent();
		}
		if (widget)
		{
			if (mOwner.Fire(event, *widget, coords))	// ����źŴ����ض���ֱ�ӽ���
				return;
		}

		// �����ǰû�о۽���widget
		if (mMouseFocus == nullptr && widget != nullptr)
		{
			MouseEnter(widget);
		}
		else if (mMouseFocus != nullptr && widget == nullptr)
		{
			MouseLeave();
		}
		else if (mMouseFocus && widget == mMouseFocus)
		{
			MouseHover(widget, coords);
		}
		else if (mMouseFocus && widget)
		{
			MouseLeave();
			MouseEnter(widget);
		}
	}
}

template<typename T>
MouseButton<T>::MouseButton(Widget & widget, Dispatcher::queue_position position):
	MouseMotion(widget, position),
	mSignalHandlerButtonDownEntered(false),
	mSignalHandlerButtonUpEntered(false),
	mIsDown(false),
	mFocus(nullptr),
	mLastClickWidget(nullptr),
	mLastClickTime(0),
	mLastDownTime(0)
{
	widget.ConnectSignal<T::buttonDownEvent>(
		std::bind(&MouseButton::mSignalHandlerButtonDown, this, std::placeholders::_2, std::placeholders::_3, std::placeholders::_5), position);

	widget.ConnectSignal<T::buttonUpEvent>(
		std::bind(&MouseButton::mSignalHandlerButtonUp, this, std::placeholders::_2, std::placeholders::_3, std::placeholders::_5), position);

}

template<typename T>
void MouseButton<T>::mSignalHandlerButtonDown(const ui_event event, bool & handle, const Point2 & coords)
{
	if (mSignalHandlerButtonDownEntered)
	{
		return;
	}
	ResourceLocker locker(mSignalHandlerButtonDownEntered);

	// ֻ����һ�ε���¼�
	if (mIsDown)
	{
		return;
	}
	mIsDown = true;

	if (mMouseCaptured)
	{
		mFocus = mMouseFocus;
	}
	else
	{
		Widget* widget = mOwner.FindAt(coords);
		while (widget && !widget->CanMouseFocus() && widget->GetParent())
		{
			widget = widget->GetParent();
		}
		if (!widget)
		{
			return;
		}
		if (mMouseFocus != widget)
		{
			mMouseFocus = widget;
		}
		mFocus = widget;
		mOwner.Fire(event, *mMouseFocus, coords);
	}

	mLastClickWidget = mFocus;
	mLastDownTime = System::Now();
	handle = true;
}

template<typename T>
void MouseButton<T>::mSignalHandlerButtonUp(const ui_event event, bool & handle, const Point2 & coords)
{
	if (mSignalHandlerButtonUpEntered)
	{
		return;
	}
	ResourceLocker locker(mSignalHandlerButtonDownEntered);

	// ֻ����һ���ɿ��¼�
	if (!mIsDown)
	{
		return;
	}
	mIsDown = false;

	// ������ڵ����widget���򴥷�up�¼�
	if (mFocus)
	{
		mOwner.Fire(event, *mMouseFocus, coords);
	}

	Widget* widget = mOwner.FindAt(coords);
	while (widget && !widget->CanMouseFocus() && widget->GetParent())
	{
		widget = widget->GetParent();
	}

	if (mMouseCaptured)
	{
		mMouseCaptured = false;
		if (mMouseFocus == widget)
		{
			MouseButtonClick(widget);
		}
		else if (!mMouseCaptured)
		{
			// ��������ڵ�ǰwidget�ɿ����򴥷�MouseLeave
			// ���������widget��ִ��MouseEnter
			MouseLeave();
			if (widget)
			{
				MouseEnter(widget);
			}
		}
	}
	else if (mFocus && mFocus == widget)
	{
		// ����ɿ�ʱ��������һ�ε����widget�ϣ�����Ϊ����
		// click����¼�
		MouseButtonClick(widget);
	}
	mFocus = nullptr;
	handle = true;
}

template<typename T>
void MouseButton<T>::MouseButtonClick(Widget * widget)
{
	uint32_t curTime = System::Now();
	if (mLastClickTime + System::clickDeltaTime >= curTime &&
		 widget == mLastClickWidget)
	{
		mLastClickWidget = nullptr;
		mLastClickTime = 0;
		mOwner.Fire(T::buttonDoubleClickEvent, *mMouseFocus, mCoords);
	}
	else if (mLastDownTime + System::clickDeltaTime >= curTime &&
		widget == mLastClickWidget)
	{
		mLastClickTime = curTime;
		mLastDownTime = 0;
		mOwner.Fire(T::buttonClickEvent, *mMouseFocus, mCoords);
	}
}

Distributor::Distributor(Widget& widget, Dispatcher::queue_position position) :
	MouseMotion(widget, position),
	MouseButtonLeft(widget, position),
	mKeyboardFocus(nullptr),
	mKeyboradFocusChain()
{
	widget.ConnectSignal<ui_event::UI_EVENT_KEY_DOWN>(
		std::bind(&Distributor::SignalHandlerKeyboradDown, this, std::placeholders::_5, std::placeholders::_6));
}

Distributor::~Distributor()
{
	mOwner.DisconnectSignal<ui_event::UI_EVENT_KEY_DOWN>(
		std::bind(&Distributor::SignalHandlerKeyboradDown, this, std::placeholders::_5, std::placeholders::_6));
}

/**
*	\brief ���widget��������Ӧ����
*/
void Distributor::AddToKeyBoardFocusChain(Widget * widget)
{
	Debug::CheckAssertion(std::find(mKeyboradFocusChain.begin(),
		mKeyboradFocusChain.end(), widget) == mKeyboradFocusChain.end(),
		"The widget has already exists in keyboard focus chain.");
	mKeyboradFocusChain.push_back(widget);
}

void Distributor::RemoveKeyBoardFocusChain(Widget * widget)
{
	auto removedWidget = std::find(mKeyboradFocusChain.begin(),
		mKeyboradFocusChain.end(), widget);

	if (removedWidget != mKeyboradFocusChain.end())
	{
		mKeyboradFocusChain.erase(removedWidget);
	}
}

/**
*	\brife ��������źŻص�
*/
void Distributor::SignalHandlerKeyboradDown(const InputEvent::KeyboardKey key, const string & unicode)
{
	for (auto ritor = mKeyboradFocusChain.rbegin(); ritor != mKeyboradFocusChain.rend(); ++ritor)
	{
		if (*ritor == mKeyboardFocus)
		{
			continue;
		}
		if (mOwner.Fire(ui_event::UI_EVENT_KEY_DOWN, **ritor, key, unicode))
		{
			return;
		}
	}
}

}
