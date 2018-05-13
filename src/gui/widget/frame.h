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

class Frame : public ContainerBase
{
public:
	Frame();
	Frame(int x, int y, int w, int h);
	~Frame();

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

	void Layout();
	void MouseCaptrue(bool mouseCpatured);
	void AddToKeyboardFocusChain(Widget* widget);
	virtual Size GetBestSize()const;

	virtual const string GetLuaObjectName()const;
	virtual const string GetControlType()const;

	void SetDistributor(std::shared_ptr<Distributor> distributor);
	virtual Widget* FindAt(const Point2& pos);

	virtual WIDGET_TYPE GetWidgetType()const
	{
		return WIDGET_TYPE::WIDGET_FRAME;
	}

	Frame* GetWindow()
	{
		return this;
	}
	const Frame* GetWindow()const
	{
		return this;
	}

private:
	status mStatus;

	int mPosX;
	int mPosY;
	int mWidth;
	int mHeight;

	bool mSuspendDrawing;
	bool mNeedLayout;

	std::shared_ptr<Distributor> mDistributor;

private:
	/******** ******* signal handlers ******* ********/

};


}