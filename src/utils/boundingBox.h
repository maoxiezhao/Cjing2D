#pragma once

#include"utils\obb.h"
#include"utils\rectangle.h"

/**
*	\brief 游戏包围盒
*
*	包围盒值维护一个OBB，但是对于很多情况需要的是AABB
*	所以会提供接口从OBB中获取AABB
*/
class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(Point2 pos, Size size);

	void SetPos(int x, int y);
	void AddPos(int dx, int dy);
	void SetSize(int w, int h);
	void SetSize(const Size& size);
	void SetRotate(float radius);
	float GetRotate()const;
	Rect GetRect()const;
	OBB GetOBB()const;
	void SetAnchor(int x, int y);

	Point2 GetPos()const;
	Size GetSize()const;
	Point2 GetCenterPos()const;

	bool Contains(int px, int py)const;
	bool Contains(const Point2& point)const;
	bool Contains(const BoundingBox& box)const;
	bool Overlaps(const BoundingBox& box)const;
	bool Intersect(const Ray& ray, float& mint, float& maxt);

private:
	void Recomput();
	
	Rect rect;
	OBB	obb;

	Point2 anchor;
	float angle;
};

inline BoundingBox::BoundingBox():
	rect(),
	obb(),
	anchor(),
	angle(0.0f)
{
}

inline BoundingBox::BoundingBox(Point2 pos, Size size):
	rect(pos, size),
	obb(pos, size),
	anchor(),
	angle(0.0f)
{
}

inline void BoundingBox::SetPos(int x, int y)
{
	rect.SetPos(x, y);
	obb.SetPos((float)x, (float)y);
}

inline void BoundingBox::AddPos(int dx, int dy)
{
	rect.AddPos(dx, dy);
	obb.AddPos((float)dx, (float)dy);
}

inline void BoundingBox::SetSize(int w, int h)
{
	rect.SetSize(w, h);
	obb.SetSize((float)w, (float)h);
}

inline void BoundingBox::SetSize(const Size & size)
{
	SetSize(size.width, size.height);
}

inline void BoundingBox::SetRotate(float radius)
{
	angle = radius;
	Recomput();
}

inline float BoundingBox::GetRotate() const
{
	return angle;
}

inline Rect BoundingBox::GetRect() const
{
	return rect;
}

inline OBB BoundingBox::GetOBB() const
{
	return OBB();
}

inline void BoundingBox::SetAnchor(int x, int y)
{
	anchor = Point2(x, y);
	Recomput();
}

inline Point2 BoundingBox::GetPos() const
{
	return rect.GetPos();
}

inline Size BoundingBox::GetSize() const
{
	return rect.GetSize();
}

inline Point2 BoundingBox::GetCenterPos() const
{
	return rect.GetCenterPos();
}

inline bool BoundingBox::Contains(int px, int py) const
{
	bool ret = rect.Contains(px, py);
	if (ret){
		if (GetRotate() != 0)
			ret = obb.Contains(px, py);
	}
	return ret;
}

inline bool BoundingBox::Contains(const Point2 & point) const
{
	bool ret = rect.Contains(point);
	if (ret){
		if (GetRotate() != 0)
			ret = obb.Contains(point);
	}
	return ret;
}

inline bool BoundingBox::Contains(const BoundingBox & box) const
{
	bool ret = rect.Contains(box.GetRect());
	if (ret) {
		if (box.GetRotate() != 0 || GetRotate() != 0)
		{
			std::vector<Vec2f> points;
			box.GetOBB().GetCorners(points);
			for (auto & point : points)
			{
				if (!obb.Contains((int)point.x, (int)point.y))
					return false;
			}
		}
	}
	return ret;
}

inline bool BoundingBox::Overlaps(const BoundingBox & box) const
{
	bool ret = rect.Overlaps(box.GetRect());
	if (ret)
	{
		if (box.GetRotate() != 0 || GetRotate() != 0)
			ret = obb.Overlaps(box.GetOBB());
	}
	return ret;
}

inline bool BoundingBox::Intersect(const Ray & ray, float & mint, float & maxt)
{
	return rect.Intersect(ray, mint, maxt);
}

inline void BoundingBox::Recomput() 
{
}


