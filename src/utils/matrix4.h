#pragma once

#include"utils\vec2.h"
#include"utils\vec3.h"
#include"utils\point.h"
#include"utils\ray.h"
#include"common\common.h"

/**
*	\brief æÿ’Û µœ÷
*/
class Matrix4
{
public:
	Matrix4();
	Matrix4(const Matrix4& other);
	Matrix4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
					  float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
	Matrix4(const float* m);

	Matrix4& operator=(const Matrix4& other);
	float& operator()(int row, int col);
	const float operator()(int row, int col)const;
	Matrix4& operator*=(const Matrix4& other);
	Matrix4& operator*=(float factor);

	bool operator== (const Matrix4& rhs);
	bool operator!= (const Matrix4& rhs);
	Matrix4 operator* (const Matrix4& rhs);
	Matrix4 operator* (float factor);

	void MakeIdentity();
	void Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
	bool MakeInverse();

	
	static Matrix4 Translate(const Vec3f& delta);
	static Matrix4 Scale(float x, float y, float z);
	static Matrix4 RotateX(float angle);
	static Matrix4 RotateY(float angle);
	static Matrix4 RotateZ(float angle);
	static Matrix4 Rotate(const Vec3f& axis, float angle);
	static Matrix4 LookAt(const Vec2f& pos, const Vec2f& look, const Vec3f& up);
	static Matrix4 Perspective(float fov, float n, float f);
	static Matrix4 Ortho(float width, float height, float zNear, float zFar);
	
	// transform
	void Transform(Vec2i& vec)const;
	void Transform(const Vec2i& in, Vec2i& out)const;
	void Transform(Vec2f& vec)const;
	void Transform(const Vec2f& in, Vec2f& out)const;
	void Transform(Vec3f& vec)const;
	void Transform(const Vec3f& in, Vec3f& out)const;
	void Transform(Point2& point)const;
	void Transform(const Point2& in, Point2& out)const;
	void Transform(Ray& ray)const;
	void Transform(const Ray& in, Ray& out)const;

private:
	union 
	{
		float  M[16];
		float _M[4][4];
	};
};
