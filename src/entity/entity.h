#ifndef _ENTITY_H_
#define _ENTITY_H_

#include"common\common.h"
#include"lua\luaObject.h"
#include"utils\point.h"
#include"utils\size.h"
#include"entity\entityInfo.h"


class Sprtie;
class LuaContext;
class Movement;
class Map;

/**
*	\brief 游戏实体的抽象类
*/
class Entity : public LuaObject
{
public:
	Entity();
	Entity(const string& name, const Point2& pos, const Size& size, int layer);

	virtual void Update();
	virtual void Draw();

	void ClearMovements();
	virtual const string GetLuaObjectName()const;

public:
	/**** **** **** status **** **** ****/
	Point2 GetPos()const;
	void SetPos(const Point2& pos);
	void SetLayer(int layer);
	int GetLayer()const;
	void SetName(const string& name);
	void SetSize(const Size& size);
	Size GetSize()const;
	const string& GetName()const;
	EntityType GetEntityType()const;

	void SetMap(Map* map);
	Map& GetMap();

private:
	// status
	string mName;
	Point2 mPos;
	Size mSize;
	int mLayer;
	EntityType mType;

	// core member
	LuaContext* mLuaContext;
	Map* mMap;

};

using EntityPtr = std::shared_ptr<Entity>;

#endif