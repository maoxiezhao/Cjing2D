#include "entity.h"
#include "game\map.h"
#include "lua\luaContext.h"

Entity::Entity():
	mName(""),
	mPos(),
	mSize(),
	mLayer(0),
	mType(EntityType::UNKNOW)
{
}

Entity::Entity(const string & name, const Point2 & pos, const Size & size, int layer):
	mName(name),
	mPos(pos),
	mSize(size),
	mLayer(layer),
	mType(EntityType::UNKNOW)
{
}

void Entity::Update()
{
}

void Entity::Draw()
{
}

void Entity::ClearMovements()
{
}

const string Entity::GetLuaObjectName() const
{
	return string();
}

/**
*	\biref 设置当前地图
*/
void Entity::SetMap(Map * map)
{
	mMap = map;
}

Map & Entity::GetMap()
{
	return *mMap;
}

Point2 Entity::GetPos()const
{
	return mPos;
}
void Entity::SetPos(const Point2& pos)
{
	mPos = pos;
}
void Entity::SetLayer(int layer)
{
	mLayer = layer;
}
int Entity::GetLayer()const
{
	return mLayer;
}
void Entity::SetName(const string& name)
{
	mName = name;
}
void Entity::SetSize(const Size & size)
{
	mSize = size;
}
Size Entity::GetSize() const
{
	return mSize;
}
const string& Entity::GetName()const
{
	return mName;
}
EntityType Entity::GetEntityType()const
{
	return mType;
}