#include"matrix4.h"
#include"geometry.h"

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

Matrix4 Matrix4::RotateX(float angle)
{
	float sint = sinf(Geometry::Radians(angle));
	float cost = cosf(Geometry::Radians(angle));
	return Matrix4 (1,0,0,0,
					0,cost, -sint,0,
					0,sint, cost, 0,
					0, 0, 0, 1);
}

Matrix4 Matrix4::RotateY(float angle)
{
	float sint = sinf(Geometry::Radians(angle));
	float cost = cosf(Geometry::Radians(angle));

	return  Matrix4(cost, 0,sint, 0,
					0,1,0,0,
					-sint, 0,cost, 0, 
					0, 0, 0, 1);
}

Matrix4 Matrix4::RotateZ(float angle)
{
	float sint = sinf(Geometry::Radians(angle));
	float cost = cosf(Geometry::Radians(angle));

	return Matrix4 (cost, -sint, 0, 0,
					sint, cost, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1);
}

/**
*	\brief 绕任一轴旋转
*/
Matrix4 Matrix4::Rotate(const Vec3f& axis, float angle)
{
	Vec3f a = axis.GetNormalized();
	float s = sinf(Geometry::Radians(angle));
	float c = cosf(Geometry::Radians(angle));
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
*	\brief 建立相机变换
*
*	这里建立相机坐标，用于将世界坐标系的转换到相机坐标
*/
Matrix4 Matrix4::LookAt(const Vec2f& pos, const Vec2f& look, const Vec3f& up) 
{
	return Matrix4();
}

/**
*	\brief 投影变换
*
*	将x,y投影到视平面上，同时将x,y,z映射为[-1.1]x[-1,1]
*/
Matrix4 Matrix4::Perspective(float fov, float n, float f)
{
	return Matrix4();
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