#include"vec3.h"

template<typename T>
constexpr Vec3<T>::Vec3():
	x(0),y(0),z(0)
{
}

template<typename T>
constexpr Vec3<T>::Vec3(T v):
	x(v),y(v),z(v)
{
}

template<typename T>
constexpr Vec3<T>::Vec3(T xx, T yy, T zz):
	x(xx),y(yy),z(zz)
{
}

template<typename T>
constexpr Vec3<T>::Vec3(const Vec2<T> & vec2, T zz)
{
	x = vec2.x; y = vec2.y; z = zz;
}

template<typename T>
inline Vec3<T> Vec3<T>::operator+(const Vec3<T>& other) const
{
	return Vec3<T>(x+other.x ,y+other.y ,z+other.z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator-(const Vec3<T>& other) const
{
	return Vec3<T>(x - other.x, y - other.y, z - other.z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator*(int factor) const
{
	return Vec3<T>(x*factor,y*factor,z*factor);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator/(int divisior) const
{
	return Vec3<T>(x/divisior,y/divisior,z/divisior);
}

template<typename T>
inline Vec3<T>& Vec3<T>::operator+=(const Vec3<T>& other)
{
	x += other.x; y += other.y; z += other.z;
	return *this;
}

template<typename T>
inline Vec3<T>& Vec3<T>::operator-=(const Vec3<T>& other)
{
	x -= other.x; y -= other.y; z -= other.z;
	return *this;
}

template<typename T>
inline Vec3<T>& Vec3<T>::operator*=(int factor)
{
	x *= factor; y *= factor; z *= factor;
	return *this;
}

template<typename T>
inline Vec3<T>& Vec3<T>::operator/=(int divisor)
{
	x /= divisor; y /= divisor; z /= divisor;
	return *this;
}

template<typename T>
inline bool Vec3<T>::operator<=(const Vec3<T>& other) const
{
	if (x == other.x)
	{
		if (y == other.y)
			return z <= other.z;
		else
			return y <= other.y;
	}
	else
		return x <= other;
}

template<typename T>
inline bool Vec3<T>::operator>=(const Vec3<T>& other) const
{
	if (x == other.x)
	{
		if (y == other.y)
			return z >= other.z;
		else
			return y >= other.y;
	}
	else
		return x >= other;
}

template<typename T>
inline bool Vec3<T>::operator==(const Vec3<T>& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

template<typename T>
inline bool Vec3<T>::operator!=(const Vec3<T>& other) const
{
	return x != other.x || y != other.y || z != other.z;
}

template<typename T>
inline Vec3<T> Vec3<T>::operator-() const
{
	return Vec3<T>(-x,-y,-z);
}

template<typename T>
inline T Vec3<T>::operator*(const Vec3<T>& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

template<typename T>
inline Vec3<T> Vec3<T>::operator^(const Vec3<T>& other) const
{
	return Vec3<T>( y*other.z - z*other.y,
					z*other.x - x*other.z,
					x*other.y - y*other.x
					);
}

template<typename T>
inline void Vec3<T>::Set(T xx, T yy, T zz)
{
	x = xx; y = yy; z = zz;
}

template<typename T>
inline float Vec3<T>::GetLength() const
{
	return sqrtf(x*x + y*y + z*z);
}

template<typename T>
inline float Vec3<T>::GetLengthSquared() const
{
	return x*x + y*y + z*z;
}

template<typename T>
inline void Vec3<T>::Normalized()
{
	float inv = 1.0f /GetLength();
	x *= inv;
	y *= inv;
	z *= inv;
}

template<typename T>
inline Vec3<T> Vec3<T>::GetNormalized() const
{
	float inv = 1.0f / GetLength();
	return Vec3<T>(x*inv, y*inv, z*inv);
}
