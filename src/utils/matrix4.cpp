#include"matrix4.h"
#include"geometry.h"


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


float & Matrix4::operator()(int row, int col)
{
	return _M[row][col];
}

const float Matrix4::operator()(int row, int col) const
{
	return _M[row][col];
}

Matrix4 & Matrix4::operator*=(const Matrix4 & other)
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

void Matrix4::MakeIdentity()
{
	for (int index = 0; index < 16; ++index)
		M[index] = 0;
	M[0] = M[5] = M[10] = M[15] = 1;
}

Matrix4 & Matrix4::operator*=(float factor)
{
	for (int index = 0; index < 16; ++index)
		M[index] *= factor;
	return *this;
}

bool Matrix4::operator==(const Matrix4 & rhs)
{
	for (int index = 0; index < 16; ++index)
		if (M[index] != rhs.M[index])
			return false;
	return true;
}

bool Matrix4::operator!=(const Matrix4 & rhs)
{
	for (int index = 0; index < 16; ++index)
		if (M[index] != rhs.M[index])
			return true;
	return false;
}

Matrix4 Matrix4::operator*(const Matrix4 & rhs)
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

Matrix4 Matrix4::operator*(float factor)
{
	Matrix4 newMat;
	for (int index = 0; index < 16; ++index)
		newMat.M[index] = M[index] * factor;
	return newMat;
}

void Matrix4::Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
	M[0] = m11;	M[1] = m12;	M[2] = m13;	M[3] = m14;
	M[4] = m21; M[5] = m22;	M[6] = m23;	M[7] = m24;
	M[8] = m31;	M[9] = m32; M[10] = m33; M[11] = m34;
	M[12] = m41; M[13] = m42; M[14] = m43; M[15] = m44;
}

/**
*	\brief ����ķ������������
*/
bool Matrix4::MakeInverse()
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
void Matrix4::Transform(Vec2i & vec) const
{
	int x = vec.x, y = vec.y;
	vec.x = (int)(M[0] * x + M[1] * y + M[2] * 0.f + M[3]);
	vec.y = (int)(M[4] * x + M[5] * y + M[6] * 0.f + M[7]);
}
void Matrix4::Transform(const Vec2i & in, Vec2i & out) const
{
	out.x = (int)(M[0] * in.x + M[1] * in.y + M[2] * 0.f + M[3]);
	out.y = (int)(M[4] * in.x + M[5] * in.y + M[6] * 0.f + M[7]);
}
void Matrix4::Transform(Vec3f& vec)const
{
	float x = vec.x, y = vec.y, z = vec.z;
	vec.x = M[0] * x + M[1] * y + M[2] * z + M[3];
	vec.y = M[4] * x + M[5] * y + M[6] * z + M[7];
	vec.z = M[8] * x + M[9] * y + M[10] * z + M[11];
}

void Matrix4::Transform(const Vec3f& in, Vec3f& out)const
{
	out.x = in.x*M[0] + in.y*M[1] + in.z*M[2] + M[3];
	out.y = in.x*M[4] + in.y*M[5] + in.z*M[6] + M[7];
	out.z = in.x*M[8] + in.y*M[9] + in.z*M[10] + M[11];
}

void Matrix4::Transform(Point2 & point) const
{
	int x = point.x, y = point.y;
	point.x = (int)(M[0] * x + M[1] * y + M[2] * 0.f + M[3]);
	point.y = (int)(M[4] * x + M[5] * y + M[6] * 0.f + M[7]);
}

void Matrix4::Transform(const Point2 & in, Point2 & out) const
{
	out.x = (int)(M[0] * in.x + M[1] * in.y + M[2] * 0.f + M[3]);
	out.y = (int)(M[4] * in.x + M[5] * in.y + M[6] * 0.f + M[7]);
}

void Matrix4::Transform(Ray & ray) const
{
//	Transform(ray.mOrgin, ray.mOrgin);
	Transform(ray.mDirection, ray.mDirection);
	ray.mDegree = Geometry::GetAngle(ray.mDirection);
}

void Matrix4::Transform(const Ray & in, Ray & out) const
{
//	Transform(in.mOrgin, out.mOrgin);
	Transform(in.mDirection, out.mDirection);
	out.mDegree = Geometry::GetAngle(out.mDirection);
}



/*************************************************************************/

Matrix4 Matrix4::Translate(const Vec3f& delta)
{
	return Matrix4( 1, 0, 0, delta.x,
					0, 1, 0, delta.y,
					0, 0, 1, delta.z,
					0, 0, 0, 1);
}

Matrix4 Matrix4::Scale(float x, float y, float z)
{
	return Matrix4( x, 0, 0, 0,
					0, y, 0, 0,
					0, 0, z, 0,
					0, 0, 0, 1);
}

/**
*	\brief ��x����ת
*	\param angle �Ƕ�degree
*/
Matrix4 Matrix4::RotateX(float angle)
{
	float sint = sinf((float)Geometry::Radians(angle));
	float cost = cosf((float)Geometry::Radians(angle));
	return Matrix4 (1,0,0,0,
					0,cost, -sint,0,
					0,sint, cost, 0,
					0, 0, 0, 1);
}

/**
*	\brief ��y����ת
*	\param angle �Ƕ�degree
*/
Matrix4 Matrix4::RotateY(float angle)
{
	float sint = sinf((float)Geometry::Radians(angle));
	float cost = cosf((float)Geometry::Radians(angle));

	return  Matrix4(cost, 0,sint, 0,
					0,1,0,0,
					-sint, 0,cost, 0, 
					0, 0, 0, 1);
}

/**
*	\brief ��z����ת
*	\param angle �Ƕ�degree
*/
Matrix4 Matrix4::RotateZ(float angle)
{
	float sint = sinf((float)Geometry::Radians(angle));
	float cost = cosf((float)Geometry::Radians(angle));

	return Matrix4 (cost, -sint, 0, 0,
					sint, cost, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
}

/**
*	\brief ����һ����ת
*/
Matrix4 Matrix4::Rotate(const Vec3f& axis, float angle)
{
	Vec3f a = axis.GetNormalized();
	float s = sinf((float)Geometry::Radians(angle));
	float c = cosf((float)Geometry::Radians(angle));
	float m[16];

	m[0] = a.x * a.x + (1.0f - a.x*a.x) * c;
	m[1] = a.x * a.y * (1.f - c) - a.z * s;
	m[2] = a.x * a.z * (1.f - c) + a.y * s;
	m[3] = 0;

	m[4] = a.x * a.y * (1.f - c) + a.z * s;
	m[5] = a.y * a.y + (1.f - a.y * a.y) * c;
	m[6] = a.y * a.z * (1.f - c) - a.x * s;
	m[7] = 0;

	m[8] = a.x * a.z * (1.f - c) - a.y * s;
	m[9] = a.y * a.z * (1.f - c) + a.x * s;
	m[10] = a.z * a.z + (1.f - a.z * a.z) * c;
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;

	return Matrix4(m);
}

/**
*	\brief ��������任
*
*	���ｨ��������꣬���ڽ���������ϵ��ת�����������
*/
Matrix4 Matrix4::LookAt(const Vec2f& pos, const Vec2f& look, const Vec3f& up) 
{
	return Matrix4();
}

/**
*	\brief ͶӰ�任
*
*	��x,yͶӰ����ƽ���ϣ�ͬʱ��x,y,zӳ��Ϊ[-1.1]x[-1,1]
*/
Matrix4 Matrix4::Perspective(float fov, float n, float f)
{
	Matrix4 persp = Matrix4(1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, f / (f - n), -f*n / (f - n),
							0, 0, 1, 0
							);
	return persp;
}

Matrix4 Matrix4::Ortho(float width, float height, float zNear, float zFar)
{
	Matrix4 mat;
	mat(0, 0) = 2.0f / width;
	mat(0, 1) = 0;
	mat(0, 2) = 0;
	mat(0, 3) = 0;

	mat(1, 0) = 0;
	mat(1, 1) = 2.0f / height;
	mat(1, 2) = 0;
	mat(1, 3) = 0;

	mat(2, 0) = 0;
	mat(2, 1) = 0;
	mat(2, 2) = 1.0f / (zFar - zNear);
	mat(2, 3) = -zNear / (zFar - zNear);

	mat(3, 0) = 0;
	mat(3, 1) = 0;
	mat(3, 2) = 0;
	mat(3, 3) = 1;

	return mat;
}