#ifndef _POINT_H_
#define _POINT_H_

#include"common\common.h"

class Point2
{
public:
	constexpr Point2();
	constexpr Point2(int x, int y);

	Point2& operator+=(const Point2& other);
	Point2& operator-=(const Point2& other);
	Point2& operator*=(int factor);
	Point2& operator/=(int divisor);

	int x = 0; 
	int y = 0;
};

constexpr bool operator==(const Point2&lhs, const Point2& rhs);
constexpr bool operator!=(const Point2&lhs, const Point2& rhs);

constexpr Point2 operator+(const Point2&lhs, const Point2& rhs);
constexpr Point2 operator-(const Point2&lhs, const Point2& rhs);

constexpr Point2 operator*(const Point2&lhs, int factor);
constexpr Point2 operator/(const Point2&lhs, int divisor);

#include"point.inl"

#endif