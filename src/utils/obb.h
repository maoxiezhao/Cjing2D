#pragma once

#include"utils\vec2.h"
#include"utils\matrix4.h"
#include"utils\size.h"

/** OBB包围盒 */
class OBB
{
public:
	OBB() :mAxis{} 
	{
		mAxis[0].x = 1.0f; mAxis[0].y = 0.0f;
		mAxis[1].x = 0.0f; mAxis[1].y = 1.0f;
	};
	OBB(const Point2& lt, const Point2& rt):mAxis{} 
	{
		mCenterPos = {float(lt.x + rt.x) * 0.5f, float(lt.y + rt.y) * 0.5f };
		mLength = { float(rt.x - lt.x) / 2, float(rt.y - lt.y) / 2};
		mAxis[0].x = 1.0f; mAxis[0].y = 0.0f;
		mAxis[1].x = 0.0f; mAxis[1].y = 1.0f;
	};
	
	OBB(const Point2& lt, const Size& size) : mAxis{}
	{
		mCenterPos = { float(lt.x + size.width / 2), float(lt.y + size.height / 2) };
		mLength = { (float)size.width / 2, (float)size.height / 2 };
		mAxis[0].x = 1.0f; mAxis[0].y = 0.0f;
		mAxis[1].x = 0.0f; mAxis[1].y = 1.0f;
	}

	OBB(const Point2& lt, const Point2& rt, const Point2& lb, const Point2& rb)
	{
		mCenterPos = { float(lt.x + rb.x) * 0.5f, float(lt.y + rb.y) * 0.5f };
		mLength = {
			(float)Geometry::GetDistance(lt, rt) * 0.5f,
			(float)Geometry::GetDistance(lt, lb) * 0.5f };
		mAxis[0] = { (float)(rt.x - lt.x), (float)(rt.y - lt.y) };
		mAxis[1] = { (float)(lb.x - lt.x), (float)(lb.y - lt.y) };
		mAxis[0].Normalized();
		mAxis[1].Normalized();
	}

	void AddPos(float x, float y)
	{
		mCenterPos.x += x;
		mCenterPos.y += y;
	}

	void SetPos(float x, float y)
	{
		float extX = mLength.x * mAxis[0].x + mLength.y * mAxis[1].x;
		float extY = mLength.x * mAxis[0].y + mLength.y * mAxis[1].y;
		mCenterPos.x = x + extX;
		mCenterPos.y = y + extY;
	}

	void SetSize(float w, float h)
	{
		float extX = mLength.x * mAxis[0].x + mLength.y * mAxis[1].x;
		float extY = mLength.x * mAxis[0].y + mLength.y * mAxis[1].y;
		Vec2f ltPos = { mCenterPos.x - extX, mCenterPos.y - extY };
		mLength = { w / 2, h / 2 };

		extX = mLength.x * mAxis[0].x + mLength.y * mAxis[1].x;
		extY = mLength.x * mAxis[0].y + mLength.y * mAxis[1].y;
		mCenterPos = { ltPos.x + extX, ltPos.y + extY };
	}

	void GetCorners(std::vector<Vec2f>& corners)const
	{
		Vec2f axisX = mAxis[0] * mLength.x;
		Vec2f axisY = mAxis[1] * mLength.y;
		Vec2f ext = axisX + axisY;

		corners.push_back( mCenterPos + axisX + axisY );
		corners.push_back( mCenterPos + axisX - axisY );
		corners.push_back( mCenterPos - axisX + axisY );
		corners.push_back( mCenterPos - axisX - axisY );
	}

	float GetProjectPoint(const Vec2f& axis, const Vec2f& pos)const
	{
		float dot = axis * pos;
		float ret = dot * (float)pos.GetLength();
		return ret;
	}

	void Transform(const Vec2f anchor, float rotate)
	{
		Vec2f offset = anchor - mLength;
		Vec2f origin = mCenterPos + offset;

		Matrix4 transform = Matrix4::Translate({ 0.0f, 0.0f, 0.0f });
		transform *= Matrix4::Translate(Vec3f(offset.x, offset.y, 0.0f));
		transform *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
		transform *= Matrix4::Translate(Vec3f(-offset.x, -offset.y, 0.0f));

		transform.Transform(offset);
		transform.Transform(mAxis[0]);
		transform.Transform(mAxis[1]);

		mAxis[0].Normalized();
		mAxis[1].Normalized();

		mCenterPos = origin - offset;
	}

	// 不处理缩放！！
	void Transform(const Matrix4& transform)
	{

	}

	void GetInterval(const OBB& box, const  Vec2f& axis, float&tmin, float&tmax)const
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