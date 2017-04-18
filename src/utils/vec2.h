#ifndef _VECTOR_2D_H_
#define _VECTOR_2D_H_

#include"common\common.h"

template <typename T>
class Vec2
{
public:
	constexpr Vec2();
	constexpr Vec2(T v);
	constexpr Vec2(T xx, T yy);

	Vec2<T> operator+(const Vec2<T>& other)const;
	Vec2<T> operator-(const Vec2<T>& other)const;
	Vec2<T> operator*(int factor)const;
	Vec2<T> operator/(int divisior)const;

	Vec2<T>&operator+= (const Vec2<T>& other);
	Vec2<T>&operator-= (const Vec2<T>& other);
	Vec2<T>&operator*= (int factor);
	Vec2<T>&operator/= (int divisor);

	bool operator <=(const Vec2<T>& other)const;
	bool operator >=(const Vec2<T>& other)const;
	bool operator ==(const Vec2<T>& other)const;
	bool operator !=(const Vec2<T>& other)const;

	// invert
	Vec2<T> operator-()const;
	// dot
	T operator*(const Vec2<T>&other)const;

	void  Set(T xx, T yy);
	float GetLength()const;
	float GetLengthSquared()const;

	Vec2<T> Normalized();

	T x, y;
};

#include"Vec2.inl"

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;

#endif