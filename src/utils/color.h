#pragma once

#include"common\common.h"

/**
*	\brief 短整型颜色结构
*/
class Color4I
{
public:
	constexpr Color4I();
	constexpr Color4I(GLushort r, GLushort g, GLushort b, GLushort a);
	
	int  GetAlpha()const;
	void SetAlpha(GLushort a);
	void GetColors(GLushort& r, GLushort& g, GLushort& b, GLushort&a)const;
	void SetColors(GLushort r, GLushort g, GLushort b, GLushort a);

	friend bool operator==(const Color4I& lhs, const Color4I& rhs);
	friend bool operator!=(const Color4I& lhs, const Color4I& rhs);

	GLushort r, g, b, a;

	static const Color4I WHITE;
	static const Color4I YELLOW;
	static const Color4I BLUE;
	static const Color4I GREEN;
	static const Color4I RED;
	static const Color4I MAGENTA;
	static const Color4I BLACK;
};

#include"color.inl"