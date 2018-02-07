#include "color.h"

constexpr Color4B::Color4B():
	r(0),g(0),b(0),a(0)
{
}

constexpr Color4B::Color4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a):
	r(r),g(g),b(b),a(a)
{
}

inline constexpr int Color4B::GetAlpha() const
{
	return a;
}

inline void Color4B::SetAlpha(GLubyte a)
{
	this->a = a;
}

inline void Color4B::GetColors(GLubyte & r, GLubyte & g, GLubyte & b, GLubyte & a) const
{
	r = this->r;
	g = this->g;
	b = this->b;
	a = this->a;
}

inline void Color4B::SetColors(GLubyte r, GLubyte g, GLubyte b, GLubyte a) 
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

inline Vec3f Color4B::GetColorRGB3F() const
{
	return Vec3f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f));
}

inline Vec3i Color4B::GetColorRGB3B() const
{
	return Vec3i((int)r, (int)g, (int)b);
}

constexpr bool operator==(const Color4B& lhs, const Color4B& rhs)
{
	return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

constexpr bool operator!=(const Color4B& lhs, const Color4B& rhs)
{
	return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b || lhs.a != rhs.a;
}