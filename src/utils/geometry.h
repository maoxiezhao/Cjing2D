#ifndef _CJING_GEOMETRY_H_
#define _CJING_GEOMETRY_H_

#include"common\common.h"

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

	float Radians(float angle)
	{
		return (float)PI * angle  / 180.0f;
	}
}

#endif