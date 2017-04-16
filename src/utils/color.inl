#include"color.h"

Color4I::Color4I():
	r(0),g(0),b(0),a(0)
{
}

inline Color4I::Color4I(GLushort r, GLushort g, GLushort b, GLushort a):
	r(r),g(g),b(b),a(a)
{
}

inline int Color4I::GetAlpha() const
{
	return a;
}

inline void Color4I::SetAlpha(GLushort a)
{
	this->a = a;
}

inline void Color4I::GetColors(GLushort & r, GLushort & g, GLushort & b, GLushort & a) const
{
	r = this->r;
	g = this->g;
	b = this->b;
	a = this->a;
}

inline void Color4I::SetColors(GLushort r, GLushort g, GLushort b, GLushort a) 
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

bool operator==(const Color4I& lhs, const Color4I& rhs)
{
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

bool operator!=(const Color4I& lhs, const Color4I& rhs)
{
	return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}