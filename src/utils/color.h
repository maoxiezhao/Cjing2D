#ifndef _COLOR_H_
#define _COLOR_H_

#include"common\common.h"

/**
*	\brief 短整型颜色结构
*/
class Color4B
{
public:
	constexpr Color4B();
	constexpr Color4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
	
	constexpr int  GetAlpha()const;
	void SetAlpha(GLubyte a);
	void GetColors(GLubyte& r, GLubyte& g, GLubyte& b, GLubyte&a)const;
	void SetColors(GLubyte r, GLubyte g, GLubyte b, GLubyte a);

	static const Color4B WHITE;
	static const Color4B YELLOW;
	static const Color4B BLUE;
	static const Color4B GREEN;
	static const Color4B RED;
	static const Color4B MAGENTA;
	static const Color4B BLACK;

private:

	friend constexpr bool operator==(const Color4B& lhs, const Color4B& rhs);
	friend constexpr bool operator!=(const Color4B& lhs, const Color4B& rhs);

	GLubyte r, g, b, a;
};

#include"color.inl"
#endif