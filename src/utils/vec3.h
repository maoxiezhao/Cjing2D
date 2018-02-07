#ifndef _VECTOR_3D_H_
#define _VECTOR_3D_H_

#include"common\common.h"
#include"utils\vec2.h"

template <typename T>
class Vec3
{
public:
	constexpr Vec3();
	constexpr Vec3(T v);
	constexpr Vec3(T xx, T yy, T zz);
	constexpr Vec3(const Vec2<T>& vec2, T zz);

	Vec3<T> operator+(const Vec3<T>& other)const;
	Vec3<T> operator-(const Vec3<T>& other)const;
	Vec3<T> operator*(int factor)const;
	Vec3<T> operator/(int divisior)const;

	Vec3<T>&operator+= (const Vec3<T>& other);
	Vec3<T>&operator-= (const Vec3<T>& other);
	Vec3<T>&operator*= (int factor);
	Vec3<T>&operator/= (int divisor);

	bool operator <=(const Vec3<T>& other)const;
	bool operator >=(const Vec3<T>& other)const;
	bool operator ==(const Vec3<T>& other)const;
	bool operator !=(const Vec3<T>& other)const;
	
	// invert
	Vec3<T> operator-()const;
	// dot
	T operator*(const Vec3<T>&other)const;
	// cross
	Vec3<T> operator^(const Vec3<T>&other)const;

	void  Set(T xx, T yy, T zz);
	float GetLength()const;
	float GetLengthSquared()const;

	void  Normalized();
	Vec3<T> GetNormalized()const;

	template<typename E>
	Vec3<E> ToVecE()
	{
		return Vec3<E>(
			static_cast<E>(x),
			static_cast<E>(y),
			static_cast<E>(z));
	}

	T x, y, z;
};

#include"vec3.inl"

using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;

#endif