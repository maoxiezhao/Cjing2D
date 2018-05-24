#pragma once

#include "entity\entity.h"

/**
*	\brief Player地图出生目标点
*
*	这个Entity用来标记Player在这个地图中的出生坐标，一个地图仅存在一个
*/
class Destination : public Entity
{
public:
	Destination(const std::string& name, const Point2& pos, int layer) :
		Entity(name, "Desination", pos, { 0, 0 }, layer) {}

	virtual EntityType GetEntityType()const
	{
		return EntityType::DESTIMATION;
	}

};