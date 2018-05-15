#ifndef _RECT_ANGLE_H_
#define _RECT_ANGLE_H_

#include"common\common.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\ray.h"

/**
*	\brief ¾ØÐÎ½á¹¹
*/
class Rect
{
public:
	constexpr Rect();
	constexpr Rect(int x,int y);
	explicit constexpr Rect(const Point2& xy);
	explicit constexpr Rect(const Size& size);
	constexpr Rect(int x, int y, int width, int height);
	constexpr Rect(const Point2&xy, const Size& size);
	constexpr Rect(const Point2&lt, const Point2&rb);

	void Set(int x1, int y1, int x2, int y2);
	void SetPos(int x, int y);
	void SetPos(const Point2& pos);
	void SetPosX(int x);
	void SetPosY(int y);
	void SetSize(int w, int h);
	void SetSize(const Size& size);
	void SetCenter(int cx, int cy);
	void SetAngle(float a);
	float GetAngle()const;

	Point2 GetPos()const;
	Size GetSize()const;
	Point2 GetCenterPos()const;
	std::vector<Point2> GetPoints()const;

	void AddX(int dx);
	void AddY(int dy);
	void AddPos(int dx, int dy);
	void AddWidth(int dw);
	void AddHeight(int dh);
	void AddSize(int dw, int dh);
	void Extend(int dx, int dy);

	bool Contains(int px, int py)const;
	bool Contains(const Point2& point)const;
	bool Contains(const Rect& rect)const;
	bool Overlaps(const Rect& rect)const;
	bool Intersect(const Ray& ray, float& mint, float& maxt);

	Rect& operator &=(const Rect& rect);
	Rect& operator |=(const Rect& rect);

	friend constexpr bool operator==(const Rect& lhs, const Rect& rhs);
	friend constexpr bool operator!=(const Rect& lhs, const Rect& rhs);
	friend constexpr bool operator<(const Rect& lhs, const Rect& rhs);
public:
	int x, y;
	int width, height;
	float angle;
};

#include"rectangle.inl"

#endif