#ifndef _ENTITY_H_
#define _ENTITY_H_

#include"common\common.h"
#include"lua\luaObject.h"
#include"utils\point.h"

class Sprtie;
class LuaContext;
class Movement;

/**
*	\brief 游戏实体的抽象类
*/
class Entity : public LuaObject
{
public:
	Entity();

	// status
	Point2 GetPos()const;
	void SetPos(const Point2& pos);

	// lua
	virtual const string GetLuaObjectName()const;

private:

	Point2 mPos;

};

using EntityPtr = std::shared_ptr<Entity>;

#endif