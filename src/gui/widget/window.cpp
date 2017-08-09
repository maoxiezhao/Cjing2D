#include "window.h"
#include "widget.h"

namespace gui
{

Window::Window()
{
}

Window::~Window()
{
}

void Window::ImplDrawBackground()
{
}

void Window::ImplDrawForeground()
{
}

void Window::ImplDrawChildren()
{
}

void Window::SetPosition(const Point2 & position)
{
}

void Window::SetSize(const Size & size)
{
}

void Window::Place(const Point2 & pos, const Size & size)
{
}

void Window::Move(const Point2 & offset)
{
}

void Window::Move(const int xoffset, const int yoffset)
{
}

void Window::InitLayout()
{
}

Size Window::GetBestSize() const
{
	return Size();
}

void Window::SetHorizontalAlignment()
{
}

void Window::SetVerticalAlignment()
{
}

Widget * Window::Find(string & id, const bool activited)
{
	return nullptr;
}

const Widget * Window::Find(string & id, const bool activied) const
{
	return nullptr;
}

bool Window::HasWidget(const Widget & widget) const
{
	return false;
}

bool Window::IsAt(const Point2 & pos) const
{
	return false;
}

}