#pragma once

#include"utils\vec2.h"
#include"utils\point.h"
#include"utils\geometry.h"

/**
*	\brief ������
*/
class Ray
{
public:
	Ray() :mMint(Geometry::EPSILON), mMaxt(FLT_MAX), mDegree(0.0f) {}
	Ray(const Point2& orgin, const Vec2i& dir, float st = Geometry::EPSILON, float et = FLT_MAX) :
		mOrgin(orgin), mDirection(dir), mMint(st), mMaxt(et), mDegree(Geometry::GetAngle(mDirection)) {}

	Point2 GetPosition(float t)const;

	Point2 mOrgin;
	Vec2i mDirection;
	float mMint, mMaxt;
	float mDegree;
};

inline Point2 Ray::GetPosition(float t)const
{
	return mOrgin + mDirection * t;
} 
