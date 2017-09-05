#include "toggleButton.h"
#include "gui\widget\window.h"

namespace gui
{

ToggleButton::ToggleButton():
	StyledWidget(),
	mState(ENABLED),
	mTestSprite(nullptr)
{
	ConnectSignal<ui_event::UI_EVENT_MOUSE_ENTER>(
		std::bind(&ToggleButton::SignalHandlerMouseEnter, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEAVE>(
		std::bind(&ToggleButton::SignalHandlerMouseLeave, this, std::placeholders::_2, std::placeholders::_3));

	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOWN>(
		std::bind(&ToggleButton::SignalHandlerMouseLeftButtonDown, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_UP>(
		std::bind(&ToggleButton::SignalHandlerMouseLeftButtonUp, this, std::placeholders::_2, std::placeholders::_3));
	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_CLICK>(
		std::bind(&ToggleButton::SignalHandlerMouseLeftButtonClick, this, std::placeholders::_2, std::placeholders::_3));

	mTestSprite = std::make_shared<Sprite>(Color4B::YELLOW, Size(400, 400));
}

void ToggleButton::SetActivite(const bool activite) const
{
}

bool ToggleButton::GetActivite() const
{
	return true;
}

unsigned int ToggleButton::GetState() const
{
	return mState;
}

void ToggleButton::SetSelected(bool selected)
{
	if (selected)
		SetState(SELECTED);
	else
		SetState(ENABLED);
}

bool ToggleButton::IsSelected() const
{
	return (mState == SELECTED);
}

void ToggleButton::ImplDrawBackground(const Point2 & offset)
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
		case SELECTED:
			mTestSprite->SetColor(Color4B::GREEN);
			break;
		default:
			break;
		}
		mTestSprite->SetPos(GetPositon());
		mTestSprite->SetSize(GetSize());
		mTestSprite->Draw();
	}
}

void ToggleButton::ImplDrawForeground(const Point2 & offset)
{
}

void ToggleButton::SignalHandlerMouseEnter(const ui_event event, bool & handle)
{
	if (mState == ENABLED)
	{
		SetState(FOCUSED);
	}
	handle = true;
}

void ToggleButton::SignalHandlerMouseLeave(const ui_event event, bool & handle)
{
	if (mState == FOCUSED)
	{
		SetState(ENABLED);
	}
	handle = true;
}

void ToggleButton::SignalHandlerMouseLeftButtonDown(const ui_event event, bool & handle)
{
	Window* windows = GetWindow();
	if (windows)
	{
		windows->MouseCaptrue(true);
	}

	handle = true;
}

void ToggleButton::SignalHandlerMouseLeftButtonUp(const ui_event event, bool & handle)
{
	SetState(ENABLED);
	handle = true;
}

void ToggleButton::SignalHandlerMouseLeftButtonClick(const ui_event event, bool & handle)
{
	SetSelected(true);
	handle = true;
}

void ToggleButton::SignalHandlerMouseLeftButtonDoubleClick(const ui_event event, bool & handle)
{
}

}
