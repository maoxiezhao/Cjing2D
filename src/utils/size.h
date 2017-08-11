#ifndef _SIZE_H_
#define _SIZE_H_

#include"common\common.h"
#include<stdint.h>

class Size
{
public:
	constexpr Size() = default;
	constexpr Size(int w, int h);

	Size& operator+=(const Size& other);
	Size& operator-=(const Size& other);
	Size& operator*=(int factor);
	Size& operator/=(int divisor);

	bool IsFlat()const;

	int width = 0;
	int height = 0;
};

constexpr bool operator==(const Size& lhs, const Size& rhs);
constexpr bool operator!=(const Size& lhs, const Size& rhs);
constexpr bool operator<=(const Size& lhs, const Size& rhs);
constexpr bool operator>=(const Size& lhs, const Size& rhs);

constexpr Size operator+(const Size& lhs,const Size& rhs);
constexpr Size operator-(const Size& lhs, const Size& rhs);
constexpr Size operator*(const Size& lhs, int factor);
constexpr Size operator*(int factor, const Size& rhs);
constexpr Size operator/(const Size& lhs,int divisor);

#include"size.inl"

#endif