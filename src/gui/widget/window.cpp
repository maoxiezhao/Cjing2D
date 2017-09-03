#include "window.h"
#include "widget.h"

namespace gui
{

/***** ****** ***** window manager ***** ****** *****/

class Manager
{
	Manager();
public:
	static Manager& GetInstance();
	
	void Add(Window& window);
	void Remove(Window& window);
	unsigned int GetID(Window& window);
	Window* GetWindow(const unsigned int id);

private:
	std::map<unsigned int, Window*> mWindows;

};

Manager::Manager() :mWindows()
{
}

Manager & Manager::GetInstance()
{
	static Manager windowManager;
	return windowManager;
}

void Manager::Add(Window & window)
{
	static unsigned int id;
	++id;
	mWindows[id] = &window;
}

void Manager::Remove(Window & window)
{
	for (auto it = mWindows.begin(); it != mWindows.end(); ++it)
	{
		if (it->second == &window)
		{
			mWindows.erase(it);
			return;
		}
	}
}

unsigned int Manager::GetID(Window & window)
{
	for (auto it = mWindows.begin(); it != mWindows.end(); ++it)
	{
		if (it->second == &window)
		{
			return it->first;
		}
	}

	return 0;
}

Window * Manager::GetWindow(const unsigned int id)
{
	auto it = mWindows.find(id);
	if (it != mWindows.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

/***** ****** ***** ***** window ***** ***** ****** *****/

Window::Window()
{
}

Window::Window(int x, int y, int w, int h) :
	ContainerBase(),
	mPosX(x),
	mPosY(y),
	mWidth(w),
	mHeight(h),
	mSuspendDrawing(true),
	mNeedLayout(true),
	mDistributor(new Distributor(*this,Dispatcher::front_child))
{
	Manager::GetInstance().Add(*this);

	Connect();

	/** 链接sign */
	ConnectSignal<gui::UI_EVENT_DRAW>(std::bind(&Window::Draw, this));

	Place({ x, y }, { w, h });
}

Window::~Window()
{
	Manager::GetInstance().Remove(*this);
}

/**
*	\brief 显示窗体
*/
void Window::Show()
{
	mSuspendDrawing = false;
}

/**
*	\brief 绘制窗体
*/
void Window::Draw()
{
	if (mSuspendDrawing)
	{
		return;
	}
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

void Window::UnDraw()
{
}

/**
*	\brief 布局操作
*/
void Window::Layout()
{
}



}