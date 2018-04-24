#pragma once

#include"utils\vec2.h"
#include"utils\matrix4.h"

/** OBB包围盒 */
class OBB
{
public:
	OBB(const Point2& lt, const Point2& rt):
		mAxis{} {};

	void GetCorners(std::vector<Vec2f>& corners)const
	{
		float extX = mLength.x * mAxis[0].x + mLength.y * mAxis[1].x;
		float extY = mLength.x * mAxis[0].y + mLength.y * mAxis[1].y;

		corners.push_back({ mCenterPos.x - extX, mCenterPos.y - extY });
		corners.push_back({ mCenterPos.x - extX, mCenterPos.y + extY });
		corners.push_back({ mCenterPos.x + extX, mCenterPos.y - extY });
		corners.push_back({ mCenterPos.x + extX, mCenterPos.y + extY });
	}

	float GetProjectPoint(const Vec2f& axis, const Vec2f& pos)const
	{
		float dot = axis * pos;
		float ret = dot * (float)pos.GetLength();
	}

	// 不处理缩放！！
	void Transform(const Matrix4& transform)
	{
		transform.Transform(mCenterPos);
		transform.Transform(mAxis[0]);
		transform.Transform(mAxis[1]);
	}

	float GetInterval(const OBB& box, const  Vec2f& axis, float&tmin, float&tmax)const
	{
		std::vector<Vec2f> corners;
		GetCorners(corners);
		float value;
		tmin = tmax = GetProjectPoint(axis, corners[0]);
		for (int i = 1; i < 4; i++)
		{
			value = GetProjectPoint(axis, corners[i]);
			tmin = std::min(tmin, value);
			tmax = std::max(tmax, value);
		}
	}

	bool Contains(int px, int py)const
	{
		float dx = (float)px - mCenterPos.x;
		float dy = (float)py - mCenterPos.y;
		if (fabsf( GetProjectPoint(mAxis[0], Vec2f(dx, dy))) > mLength.x)
			return false;
		if (fabsf(GetProjectPoint(mAxis[1], Vec2f(dx, dy))) > mLength.y)
			return false;
		return true;
	}
	
	bool Contains(const Point2& point)const
	{
		return Contains(point.x, point.y);
	}
	
	bool Overlaps(const OBB& box)const
	{
		float min1, max1, min2, max2;
		for (int i = 0; i < 2; i++)
		{
			GetInterval(*this, mAxis[i], min1, max1);
			GetInterval(box, mAxis[i], min2, max2);
			if (max1 < min2 || max2 < min1)return false;
		}
		for (int i = 0; i < 2; i++)
		{
			GetInterval(*this, box.mAxis[i], min1, max1);
			GetInterval(box, box.mAxis[i], min2, max2);
			if (max1 < min2 || max2 < min1)return false;
		}
		return true;
	}
public:
	Vec2f mAxis[2];
	Vec2f mLength;
	Vec2f mCenterPos;

};