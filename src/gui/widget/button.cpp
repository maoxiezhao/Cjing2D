#include "button.h"

namespace gui
{

Button::Button():
	StyledWidget(),
	mState(ENABLED)
{
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

}
