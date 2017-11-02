#pragma once

#include"common\common.h"
#include"gui\core\distributor.h"
#include"gui\widget\widget.h"
#include"gui\widget\containerBase.h"

/**
*	\brief 窗体基类
*/

namespace gui
{
class Widget;
class Distributor;

class Window : public ContainerBase
{
public:
	Window();
	Window(int x, int y, int w, int h);
	~Window();

	void Show(bool showed);
	void Draw();
	void UnDraw();

	/** 窗体的状态 */
	enum status 
	{
		NEW,
		SHOWING,
		REQUEST_CLOSE,
		CLOSED
	};

	Window* GetWindow()
	{
		return this;
	}
	const Window* GetWindow()const
	{
		return this;
	}

	void Layout();

	virtual const string GetControlType()const;

	// capture event
	void MouseCaptrue(bool mouseCpatured);

	void AddToKeyboardFocusChain(Widget* widget);

private:
	status mStatus;

	int mPosX;
	int mPosY;
	int mWidth;
	int mHeight;

	bool mSuspendDrawing;
	bool mNeedLayout;

	std::unique_ptr<Distributor> mDistributor;

private:
	/******** ******* signal handlers ******* ********/

};


}