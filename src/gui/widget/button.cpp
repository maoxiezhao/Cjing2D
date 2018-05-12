#include "button.h"
#include "window.h"

namespace gui
{
namespace
{
	const Color4B testEnabledColor = Color4B::BLACK;
	const Color4B testFocusColor  = Color4B::YELLOW;
}

Button::Button():
	StyledWidget(),
	mState(ENABLED),
	mTestSprite(nullptr)
{
	ConnectSignal<ui_event::UI_EVENT_MOUSE_ENTER>(
		std::bind(&Button::SignalHandlerMouseEnter, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEAVE>(
		std::bind(&Button::SignalHandlerMouseLeave, this, std::placeholders::_2, std::placeholders::_3));
	
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOWN>(
		std::bind(&Button::SignalHandlerMouseLeftButtonDown, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_UP>(
		std::bind(&Button::SignalHandlerMouseLeftButtonUp, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_CLICK>(
		std::bind(&Button::SignalHandlerMouseLeftButtonClick, this, std::placeholders::_2, std::placeholders::_3));

	mTestSprite = std::make_shared<Sprite>(testFocusColor, Size(400, 400));

	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOUBLE_CLICK>(
		std::bind(&Button::SignalHandlerMouseLeftButtonDoubleClick, this, std::placeholders::_2, std::placeholders::_3));
}

void Button::SetActivite(const bool activite) const
{
}

bool Button::GetActivite() const
{
	return false;
}

unsigned int Button::GetState() const
{
	return 0;
}

void Button::ImplDrawBackground(const Point2 & offset)
{
	// test state drawing
	if (mTestSprite != nullptr)
	{
		switch (mState)
		{
		case ENABLED:
			mTestSprite->SetColor(Color4B::BLUE);
			break;
		case DISABLED:
			break;
		case FOCUSED:
			mTestSprite->SetColor(Color4B::WHITE);
			break;
		case PRESSED:
			mTestSprite->SetColor(Color4B::YELLOW);
			break;
		default:
			break;
		}
		mTestSprite->SetPos(GetPositon());
		mTestSprite->SetSize(GetSize());
		//mTestSprite->SetColor(Color4B::BLUE);
		mTestSprite->Draw();
	}
}

void Button::ImplDrawForeground(const Point2 & offset)
{

}

void Button::SignalHandlerMouseEnter(const  ui_event event, bool & handle)
{
	SetState(FOCUSED);
	handle = true;
	std::cout << "Enter" << std::endl;
}

void Button::SignalHandlerMouseLeave(const  ui_event event, bool & handle)
{
	SetState(ENABLED);
	handle = true;
}

void Button::SignalHandlerMouseLeftButtonDown(const ui_event event, bool & handle)
{
	SetState(PRESSED);
	Window* windows = GetWindow();
	if (windows)
	{
		windows->MouseCaptrue(true);
	}

	handle = true;
}

void Button::SignalHandlerMouseLeftButtonUp(const ui_event event, bool & handle)
{
	SetState(FOCUSED);
	handle = true;
}

void Button::SignalHandlerMouseLeftButtonClick(const ui_event event, bool & handle)
{
	std::cout << "Click Event" << endl;
}

void Button::SignalHandlerMouseLeftButtonDoubleClick(const ui_event event, bool & handle)
{
	std::cout << "Double Click Event" << endl;
}


}
