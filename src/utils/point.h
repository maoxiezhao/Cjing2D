#ifndef _POINT_H_
#define _POINT_H_

#include"common\common.h"
#include"utils\vec2.h"

class Point2
{
public:
	constexpr Point2();
	constexpr Point2(const Vec2i& vec);
	constexpr Point2(int x, int y);

	const Point2 operator-()const;
	Point2& operator+=(const Point2& other);
	Point2& operator-=(const Point2& other);
	Point2& operator*=(int factor);
	Point2& operator/=(int divisor);

	int x = 0; 
	int y = 0;
};

constexpr bool operator==(const Point2&lhs, const Point2& rhs);
constexpr bool operator!=(const Point2&lhs, const Point2& rhs);
constexpr bool operator<(const Point2&lhs, const Point2& rhs);

constexpr Point2 operator+(const Point2&lhs, const Point2& rhs);
//constexpr Point2 operator-(const Point2&lhs, const Point2& rhs);
inline Vec2i operator-(const Point2&lhs, const Point2& rhs);

constexpr Point2 operator*(const Point2&lhs, int factor);
constexpr Point2 operator/(const Point2&lhs, int divisor);

#include"point.inl"

#endif