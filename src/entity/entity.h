#ifndef _ENTITY_H_
#define _ENTITY_H_

#include"common\common.h"
#include"lua\luaObject.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"
#include"entity\entityInfo.h"
#include"game\sprite.h"

class Sprtie;
class LuaContext;
class Movement;
class Map;
class EntityState;
class GameCommand;

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
	virtual void Initalized();

	void ClearMovements();
	virtual const string GetLuaObjectName()const;

	/**** **** **** Notify **** **** ****/
	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);

	void NotifyMapStarted();

public:
	/**
	*	带名字属性的share_ptr<sprite>
	*/
	struct NamedSpritePtr
	{
		std::string name;
		SpritePtr sprite;
	};

	/**** **** **** status **** **** ****/
	Point2 GetPos()const;
	void SetPos(const Point2& pos);
	void SetLayer(int layer);
	int GetLayer()const;
	void SetName(const string& name);
	void SetSize(const Size& size);
	Size GetSize()const;
	const string& GetName()const;
	virtual EntityType GetEntityType()const;
	Rect GetRectBounding()const;

	void SetMap(Map* map);
	Map& GetMap();
	
	/**** ***** state manager ***** ****/
	const std::shared_ptr<EntityState>& GetState();
	void SetState(const std::shared_ptr<EntityState>& state);
	void UpdateState();

	/***** **** movement manager ***** *****/
	void StopMovement();
	void StartMovement(const std::shared_ptr<Movement>& movement);
	const std::shared_ptr<Movement>& GetMovement();

private:
	// status
	string mName;
	Point2 mPos;
	Size mSize;
	int mLayer;
	EntityType mType;
	bool mIsInitialized;

	// core member
	LuaContext* mLuaContext;				/** 当前的luaContext */
	Map* mMap;								/** 当前entity所属的map */
	std::shared_ptr<EntityState> mState;	/** entity 的状态组件 */
	std::shared_ptr<Movement> mMovement;	/** entity 的运动组件 */

	std::vector<NamedSpritePtr> mSprites;	/** entity 所拥有的用于展示的sprites */
};

using EntityPtr = std::shared_ptr<Entity>;

#endif