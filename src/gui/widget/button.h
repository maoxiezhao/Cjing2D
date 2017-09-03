#pragma once

#include"gui\widget\styledwidget.h"

namespace gui
{

class Button : public StyledWidget
{
public:
	Button();

	virtual void SetActivite(const bool activite)const;
	virtual bool GetActivite()const;
	virtual unsigned int GetState()const;

private:
	enum State
	{
		ENABLED,
		DISABLED
	};

	void SetState(const State state);

	State mState;
};


}