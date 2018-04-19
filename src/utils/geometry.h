#ifndef _CJING_GEOMETRY_H_
#define _CJING_GEOMETRY_H_

#include"common\common.h"
#include"utils\point.h"
#include"utils\vec2.h"

/**
*	\brief 数学常用定义和常用的数学函数
*/
namespace Geometry
{
	constexpr double PI			 = 3.14159265358979323846;
	constexpr double PI_2		 = 6.28318530717958647693;
	constexpr double INV_PI		 = 0.31830988618379067154;
	constexpr double INV_TWOPI   = 0.15915494309189533577;
	constexpr double INV_FOURPI  = 0.07957747154594766788;
	constexpr double PI_OVER_2   = 1.57079632679489661923;
	constexpr double PI_OVER_4   = 0.78539816339744830961;

	constexpr float  EPSILON = 1e-3f;

	inline double Radians(double angle)
	{
		return PI * angle / 180.0f;
	}

	inline float Degree(float radians)
	{
		return (float)(radians / PI * 180.0f);
	}

	inline double GetDistance(const Point2& p1, const Point2& p2)
	{
		double dx = (double)(p2.x - p1.x);
		double dy = (double)(p2.y - p1.y);
		return sqrt(dx * dx + dy * dy);
	}

	inline double GetAngle(double x1,double y1,double x2,double y2)
	{
		double dx = x2 - x1;
		double dy = y2 - y1;
		
		// 当都为0时，则直接返回90度
		if (dx == 0.0 && dy == 0.0)
		{
			return PI_OVER_2;
		}
		double angle = std::atan2(-dy, dx);
		if (angle < 0.0)
		{
			angle += PI_2;
		}
		return angle;
	}

	inline double GetAngle(const Point2& p1, const Point2& p2)
	{
		Point2 dxy = p2 - p1;
		if (dxy.x == 0 && dxy.y == 0)
		{
			return PI_OVER_2;
		}
		double angle = std::atan2(-dxy.y, dxy.x);
		if (angle < 0.0)
		{
			angle += PI_2;
		}
		return angle;
	}

	inline double GetAngle(const Vec2d& vec)
	{
		if (vec.x == 0 && vec.y == 0)
		{
			return PI_OVER_2;
		}
		double angle = std::atan2(-vec.y, vec.x);
		if (angle < 0.0)
		{
			angle += PI_2;
		}
		return angle;
	}

	inline float GetAngle(const Vec2i& vec)
	{
		if (vec.x == 0 && vec.y == 0)
		{
			return (float)PI_OVER_2;
		}
		float angle = std::atan2((float)-vec.y, (float)vec.x);
		if (angle < 0.0)
		{
			angle += (float)PI_2;
		}
		return angle;
	}
}

#endif