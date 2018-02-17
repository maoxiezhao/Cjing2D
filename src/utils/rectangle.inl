#include "rectangle.h"
constexpr Rect::Rect():
	x(0),y(0),
	width(0),height(0)
{
}

constexpr Rect::Rect(int x, int y):
	x(x), y(y),
	width(0), height(0)
{
}

constexpr Rect::Rect(const Point2& xy) :
	x(xy.x), y(xy.y),
	width(0), height(0)
{
}

constexpr Rect::Rect(const Size& size):
	x(0),y(0),
	width(size.width),
	height(size.height)
{
}

constexpr Rect::Rect(int x, int y, int width, int height):
	x(x),y(y),
	width(width),height(height)
{
}

constexpr Rect::Rect(const Point2&xy, const Size& size):
	x(xy.x),
	y(xy.y),
	width(size.width),
	height(size.height)
{
}

constexpr Rect::Rect(const Point2&lt, const Point2&rb):
	x(lt.x),
	y(lt.y),
	width(rb.x - lt.x),
	height(rb.y - lt.y)
{
}

inline void Rect::Set(int x1, int y1, int x2, int y2)
{
	x = x1;
	y = y1;
	width = x2 - x1;
	height = y2 - y1;
}

inline void Rect::SetPos(int x, int y)
{
	this->x = x;
	this->y = y;
}

inline void Rect::SetPosX(int x)
{
	this->x = x;
}

inline void Rect::SetPosY(int y)
{
	this->y = y;
}

inline void Rect::SetSize(int w, int h)
{
	width = w;
	height = h;
}

inline void Rect::SetCenter(int cx, int cy)
{
	x = cx - width / 2;
	y = cy - height / 2;
}

inline Point2 Rect::GetPos() const
{
	return Point2(x, y);
}

inline Size Rect::GetSize() const
{
	return Size(width, height);
}

inline Point2 Rect::GetCenterPos() const
{
	return Point2(x + width / 2, y + height / 2);
}

inline std::vector<Point2> Rect::GetPoints() const
{
	return {{ x,y }, 
			{x + width, y},
			{x, y + height},
			{x + width, y + height} };
}

inline void Rect::AddX(int dx)
{
	x += dx;
}

inline void Rect::AddY(int dy)
{
	y += dy;
}

inline void Rect::AddPos(int dx, int dy)
{
	x += dx; 
	y += dy;
}

inline void Rect::AddWidth(int dw)
{
	width += dw;
}

inline void Rect::AddHeight(int dh)
{
	height += dh;
}

inline void Rect::AddSize(int dw, int dh)
{
	width += dw;
	height += dh;
}

inline void Rect::Extend(int dx, int dy)
{
	AddSize(dx, dy);
	AddPos(-dx / 2, -dy / 2);
}

inline bool Rect::Contains(int px, int py) const
{
	return px >= x && px <= (x + width) &&
		py >= y && py <= (y + height);
}

inline bool Rect::Contains(const Point2 & point) const
{
	return point.x >= x && point.x <= (x + width) &&
		point.y >= y && point.y <= (y + height);
}

inline bool Rect::Contains(const Rect & rect) const
{
	return rect.x >= x && (rect.x + rect.width)  <= (x + width) &&
		   rect.y >= y && (rect.y + rect.height) <= (y + height);
}

inline bool Rect::Overlaps(const Rect & rect) const
{
	return !(x + width < rect.x || x > rect.x +rect.width ||
		y + height < rect.y || y > rect.y + rect.height);
}

/**
*	\brief 射线相交检测
*	\param mint 得到的最小t
*	\param maxt 得到的最大t
*	\return bool 是否发生相交
*
*	基本思路就是对每一轴计算最大和最小的t值，当发生记录
*	最小的t值大于记录最大的，说明未相交
*/
inline bool Rect::Intersect(const Ray & ray, float & mint, float & maxt)
{
	float t1 = ray.mMint, t2 = ray.mMaxt;
	auto CheckIntesertAsix = [&t1, &t2](int origin, int dir, int dstMin, int dstMax)->bool {
		float invDir = float(1.0f / dir);
		float tNear = (dstMin - origin) * invDir;
		float tFar = (dstMax - origin) * invDir;

		if (tNear > tFar)
			std::swap(tNear, tFar);

		t1 = std::max(tNear, t1);
		t2 = std::min(tFar, t2);

		if (t1 > t2)
			return false;
		return true;
	};

	if (!CheckIntesertAsix(ray.mOrgin.x, ray.mDirection.x, x, x + width) ||
		!CheckIntesertAsix(ray.mOrgin.y, ray.mDirection.y, y, y + height) )
		return false;

	// 首先判断光线原点,是否在rect中，如果在则必然发生相交
	if (Contains(ray.mOrgin))
	{
		mint = t2;
		maxt = t1;
	}
	else
	{
		mint = t1;
		maxt = t2;
	}

	return true;
}

constexpr bool operator==(const Rect& lhs, const Rect& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y &&
		lhs.width == rhs.width && lhs.height == rhs.height;
}

constexpr bool operator!=(const Rect& lhs, const Rect& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y ||
		lhs.width != rhs.width || lhs.height != rhs.height;
}

constexpr bool operator<(const Rect& lhs, const Rect& rhs)
{
	return lhs.x < rhs.x && lhs.y < rhs.y &&
		lhs.width < rhs.width && lhs.height < rhs.height;
}

/**
*	\brief 返回两个rect的相交区域
*
*    如果存在相交，则以rect返回相交区域
*    如果不存在相交，则返回原点在起始位置，大小为0的rects
*/
inline Rect& Rect::operator&=(const Rect& rect)
{
	int lx = std::max(x, rect.x);
	int ly = std::max(y, rect.y);
	int rx = std::min(x + width, rect.x + rect.width);
	int ry = std::min(y + height, rect.y + rect.height);

	if (lx >= rx || ly >= ry)
		*this = Rect();
	else
		*this = Rect(Point2(lx, ly), Point2(rx, ry));
	return *this;
}

/**
*	\brief 返回两个rect的并区域
*/
inline Rect& Rect::operator |=(const Rect& rect)
{
	int lx = std::min(x, rect.x);
	int ly = std::min(y, rect.y);
	int rx = std::max(x + width, rect.x + rect.width);
	int ry = std::max(y + height, rect.y + rect.height);

	if (lx >= rx || ly >= ry)
		*this = Rect();
	else
		*this = Rect(Point2(lx, ly), Point2(rx, ry));
	return *this;
}

