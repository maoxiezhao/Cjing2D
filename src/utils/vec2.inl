#include"vec2.h"

template<typename T>
constexpr Vec2<T>::Vec2() :
	x(0), y(0)
{
}

template<typename T>
constexpr Vec2<T>::Vec2(T v) :
	x(v), y(v)
{
}

template<typename T>
constexpr Vec2<T>::Vec2(T xx, T yy) :
	x(xx), y(yy)
{
}

template<typename T>
inline Vec2<T> Vec2<T>::operator+(const Vec2<T>& other) const
{
	return Vec2<T>(x + other.x, y + other.y);
}

template<typename T>
inline Vec2<T> Vec2<T>::operator-(const Vec2<T>& other) const
{
	return Vec2<T>(x - other.x, y - other.y);
}

template<typename T>
inline Vec2<T> Vec2<T>::operator*(int factor) const
{
	return Vec2<T>(x*factor, y*factor);
}

template<typename T>
inline Vec2<T> Vec2<T>::operator/(int divisior) const
{
	return Vec2<T>(x / divisior, y / divisior);
}

template<typename T>
inline Vec2<T>& Vec2<T>::operator+=(const Vec2<T>& other)
{
	x += other.x; y += other.y;
	return *this;
}

template<typename T>
inline Vec2<T>& Vec2<T>::operator-=(const Vec2<T>& other)
{
	x -= other.x; y -= other.y; 
	return *this;
}

template<typename T>
inline Vec2<T>& Vec2<T>::operator*=(int factor)
{
	x *= factor; y *= factor; 
	return *this;
}

template<typename T>
inline Vec2<T>& Vec2<T>::operator/=(int divisor)
{
	x /= divisor; y /= divisor; 
	return *this;
}

template<typename T>
inline bool Vec2<T>::operator<=(const Vec2<T>& other) const
{
	if (x == other.x)
		return y <= other.y;
	else
		return x <= other;
}

template<typename T>
inline bool Vec2<T>::operator>=(const Vec2<T>& other) const
{
	if (x == other.x)
		return y >= other.y;
	else
		return x >= other;
}

template<typename T>
inline bool Vec2<T>::operator==(const Vec2<T>& other) const
{
	return x == other.x && y == other.y;
}

template<typename T>
inline bool Vec2<T>::operator!=(const Vec2<T>& other) const
{
	return x != other.x || y != other.y;
}

template<typename T>
inline Vec2<T> Vec2<T>::operator-() const
{
	return Vec2<T>(-x, -y);
}

template<typename T>
inline T Vec2<T>::operator*(const Vec2<T>& other) const
{
	return x*other.x + y*other.y;
}

template<typename T>
inline void Vec2<T>::Set(T xx, T yy)
{
	x = xx; y = yy;
}

template<typename T>
inline double Vec2<T>::GetLength() const
{
	return sqrtf(x*x + y*y);
}

template<typename T>
inline double Vec2<T>::GetLengthSquared() const
{
	return x*x + y*y;
}

template<typename T>
inline Vec2<T> Vec2<T>::Normalized()
{
	float inv = 1.0f / GetLength();
	x *= inv;
	y *= inv;
	return *this;
}
