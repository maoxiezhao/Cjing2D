#include "entity.h"

Entity::Entity()
{
}

Point2 Entity::GetPos() const
{
	return Point2();
}

void Entity::SetPos(const Point2 & pos)
{
}

void Entity::ClearMovements()
{
}

const string Entity::GetLuaObjectName() const
{
	return string();
}
