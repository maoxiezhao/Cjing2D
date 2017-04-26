/*

#include"matrix4.h"

Matrix4::Matrix4()
{
	memset(M, 0, sizeof(float) * 16);
}

Matrix4::Matrix4(const Matrix4& other)
{
	memcpy(M, other.M, sizeof(float) * 16);
}

Matrix4::Matrix4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	Set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

Matrix4::Matrix4(const float * m)
{
	memcpy(M, m, sizeof(float) * 16);
}


Matrix4 & Matrix4::operator=(const Matrix4 & other)
{
	memcpy(M, other.M, sizeof(float) * 16);
	return *this;
}


inline float & Matrix4::operator()(int row, int col)
{
	return _M[row][col];
}

inline const float Matrix4::operator()(int row, int col) const
{
	return _M[row][col];
}

inline Matrix4 & Matrix4::operator*=(const Matrix4 & other)
{
	Matrix4 temp;
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			temp._M[row][col] = _M[row][0] * other._M[0][col] +
			_M[row][1] * other._M[1][col] +
			_M[row][2] * other._M[2][col] +
			_M[row][3] * other._M[3][col];
	*this = temp;
	return *this;
}

inline void Matrix4::MakeIdentity()
{
	for (int index = 0; index < 16; ++index)
		M[index] = 0;
	M[0] = M[5] = M[10] = M[15] = 1;
}

inline Matrix4 & Matrix4::operator*=(float factor)
{
	for (int index = 0; index < 16; ++index)
		M[index] *= factor;
	return *this;
}

inline bool Matrix4::operator==(const Matrix4 & rhs)
{
	for (int index = 0; index < 16; ++index)
		if (M[index] != rhs.M[index])
			return false;
	return true;
}

inline bool Matrix4::operator!=(const Matrix4 & rhs)
{
	for (int index = 0; index < 16; ++index)
		if (M[index] != rhs.M[index])
			return true;
	return false;
}

inline Matrix4 Matrix4::operator*(const Matrix4 & rhs)
{
	Matrix4 temp;
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			temp._M[row][col] = _M[row][0] * rhs._M[0][col] +
			_M[row][1] * rhs._M[1][col] +
			_M[row][2] * rhs._M[2][col] +
			_M[row][3] * rhs._M[3][col];
	return temp;
}

inline Matrix4 Matrix4::operator*(float factor)
{
	Matrix4 newMat;
	for (int index = 0; index < 16; ++index)
		newMat.M[index] = M[index] * factor;
	return newMat;
}

inline void Matrix4::Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	M[0] = m11;	M[1] = m12;	M[2] = m13;	M[3] = m14;
	M[4] = m21; M[5] = m22;	M[6] = m23;	M[7] = m24;
	M[8] = m31;	M[9] = m32; M[10] = m33; M[11] = m34;
	M[12] = m41; M[13] = m42; M[14] = m43; M[15] = m44;
}
*/

/**
*	\brief 克拉姆法则计算逆矩阵
*//*
inline bool Matrix4::MakeInverse()
{
	const Matrix4 &m = *this;

	float d = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) - (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3))
		+ (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)) + (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3))
		- (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3));
	if (d == 0.f)
		return false;
	else
	{
		d = 1.f / d;
		Matrix4 temp;
		temp(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) + m(2, 1) * (m(3, 2) * m(1, 3) - m(1, 2) * m(3, 3)) + m(3, 1) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)));
		temp(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3)) + m(2, 2) * (m(3, 0) * m(1, 3) - m(1, 0) * m(3, 3)) + m(3, 2) * (m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3)));
		temp(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) + m(2, 3) * (m(3, 0) * m(1, 1) - m(1, 0) * m(3, 1)) + m(3, 3) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)));
		temp(3, 0) = d * (m(1, 0) * (m(3, 1) * m(2, 2) - m(2, 1) * m(3, 2)) + m(2, 0) * (m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2)) + m(3, 0) * (m(2, 1) * m(1, 2) - m(1, 1) * m(2, 2)));
		temp(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) + m(3, 1) * (m(2, 2) * m(0, 3) - m(0, 2) * m(2, 3)) + m(0, 1) * (m(3, 2) * m(2, 3) - m(2, 2) * m(3, 3)));
		temp(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(3, 0) * m(0, 3)) + m(3, 2) * (m(2, 0) * m(0, 3) - m(0, 0) * m(2, 3)) + m(0, 2) * (m(3, 0) * m(2, 3) - m(2, 0) * m(3, 3)));
		temp(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) + m(3, 3) * (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) + m(0, 3) * (m(3, 0) * m(2, 1) - m(2, 0) * m(3, 1)));
		temp(3, 1) = d * (m(2, 0) * (m(3, 1) * m(0, 2) - m(0, 1) * m(3, 2)) + m(3, 0) * (m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2)) + m(0, 0) * (m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2)));
		temp(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3)) + m(0, 1) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) + m(1, 1) * (m(3, 2) * m(0, 3) - m(0, 2) * m(3, 3)));
		temp(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(1, 0) * m(0, 3)) + m(0, 2) * (m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3)) + m(1, 2) * (m(3, 0) * m(0, 3) - m(0, 0) * m(3, 3)));
		temp(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) + m(0, 3) * (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) + m(1, 3) * (m(3, 0) * m(0, 1) - m(0, 0) * m(3, 1)));
		temp(3, 2) = d * (m(3, 0) * (m(1, 1) * m(0, 2) - m(0, 1) * m(1, 2)) + m(0, 0) * (m(3, 1) * m(1, 2) - m(1, 1) * m(3, 2)) + m(1, 0) * (m(0, 1) * m(3, 2) - m(3, 1) * m(0, 2)));
		temp(0, 3) = d * (m(0, 1) * (m(2, 2) * m(1, 3) - m(1, 2) * m(2, 3)) + m(1, 1) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + m(2, 1) * (m(1, 2) * m(0, 3) - m(0, 2) * m(1, 3)));
		temp(1, 3) = d * (m(0, 2) * (m(2, 0) * m(1, 3) - m(1, 0) * m(2, 3)) + m(1, 2) * (m(0, 0) * m(2, 3) - m(2, 0) * m(0, 3)) + m(2, 2) * (m(1, 0) * m(0, 3) - m(0, 0) * m(1, 3)));
		temp(2, 3) = d * (m(0, 3) * (m(2, 0) * m(1, 1) - m(1, 0) * m(2, 1)) + m(1, 3) * (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) + m(2, 3) * (m(1, 0) * m(0, 1) - m(0, 0) * m(1, 1)));
		temp(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) + m(1, 0) * (m(2, 1) * m(0, 2) - m(0, 1) * m(2, 2)) + m(2, 0) * (m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2)));
		*this = temp;
		return true;
	}
}


inline void Matrix4::Transform(Vec3f& vec)const
{
	float x = vec.x, y = vec.y, z = vec.z;
	vec.x = M[0] * x + M[1] * y + M[2] * z + M[3];
	vec.y = M[4] * x + M[5] * y + M[6] * z + M[7];
	vec.z = M[8] * x + M[9] * y + M[10] * z + M[11];
}

inline void Matrix4::Transfomr(const Vec3f& in, Vec3f& out)const
{
	out.x = in.x*M[0] + in.y*M[1] + in.z*M[2] + M[3];
	out.y = in.x*M[4] + in.y*M[5] + in.z*M[6] + M[7];
	out.z = in.x*M[8] + in.y*M[9] + in.z*M[10] + M[11];
}
*/