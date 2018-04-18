#ifndef _ENTITY_H_
#define _ENTITY_H_

#include"common\common.h"
#include"lua\luaObject.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"
#include"entity\entityInfo.h"
#include"game\sprite.h"
#include"game\gameCommands.h"
#include"game\animationSprite.h"

class Sprtie;
class LuaContext;
class Movement;
class Map;
class EntityState;

/**
*	\brief 游戏实体的抽象类
*/
class Entity : public LuaObject
{
public:
	/**
	*	带名字属性的share_ptr<sprite>
	*/
	struct NamedSpritePtr
	{
		std::string name;
		SpritePtr sprite;
		bool removed;
	};

	/**
	*	\brief 碰撞模式
	*/
	enum CollisionMode
	{
		COLLISION_NONE      = 0x00,     // 无碰撞模式，不参与碰撞
		COLLISION_OVERLAPING = 0x01,	// 覆盖模式
		COLLISION_CONTAINING = 0x02,	// 包含模式
	};

	Entity();
	Entity(const string& name, const Point2& pos, const Size& size, int layer);
	~Entity();

	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

	void DrawDebugBounding();
	void ClearMovements();

	/** Lua Context */
	virtual const string GetLuaObjectName()const;

	/**** **** **** Notify **** **** ****/
	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);
	virtual void NotifyMovementChanged();
	virtual void NotifyPositonChanged();
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual void NotifyBeRemoved();
	virtual void NotifyFacingEntityChanged(Entity* entity);
	virtual void NotifyOverlapEntityChanged(Entity* entity);
	
	// game command notify
	virtual bool NotifyCommandInteractPressed(Entity& interactEntity);

	void NotifyMapStarted();
	
public:
	/**** **** **** status **** **** ****/
	Point2 GetPos()const;
	Point2 GetCenterPos()const;
	Point2 GetLeftTopPos()const;
	void SetPos(const Point2& pos);
	void SetLayer(int layer);
	int GetLayer()const;
	void SetName(const string& name);
	void SetSize(const Size& size);
	Size GetSize()const;
	const string& GetName()const;
	Rect GetRectBounding()const;
	void SetDrawOnYOrder(bool isDrawOnY);
	bool IsDrawOnYOrder()const;
	void SetOrigin(const Point2& origin);
	const Point2& GetOrigin()const;
	Point2 GetScreenPos()const;
	bool IsVisible()const;
	void SetVisible(bool visibled);
	bool IsBeRemoved()const;
	virtual EntityType GetEntityType()const;
	virtual float GetFacingDegree()const;

	// special status
	void SetFacingEntity(Entity* entity);
	Entity* GetFacingEntity();
	void SetOverlapEntity(Entity* entity);
	Entity* GetOverlapEntity();
	void SetFocused(bool bFocused);
	bool IsFosused()const;

	void SetMap(Map* map);
	Map& GetMap();
	bool IsOnMap()const;
	const Map& GetMap()const;
	void RemoveFromMap();
	Game& GetGame();

	/**** ***** sprite ***** ****/
	SpritePtr CreateSprite(const string & spriteName);
	AnimationSpritePtr CreateAnimationSprite(const string & animationSetId, const string & animationID);
	SpritePtr GetSprite(const string& spriteName);
	bool RemoveSprite(SpritePtr sprite);
	void ClearSprites();
	void ClearRemovedSprite();

	/**** ***** state manager ***** ****/
	EntityState& GetState()const;
	void SetState(EntityState* state);
	void UpdateState();

	/***** **** movement manager ***** *****/
	void StopMovement();
	void StartMovement(const std::shared_ptr<Movement>& movement);
	const std::shared_ptr<Movement>& GetMovement();

	/***** **** collision ***** *****/
	void CheckCollisionWithEntities();
	virtual void CheckCollision(Entity& otherEntity);

	bool TestCollisionWithRect(const Entity& entity);
	bool TestCollisionContaining(const Entity& entity);

	bool IsHaveCollision()const;
	void SetCollisionMode(CollisionMode collisionMode);
	bool HasCollisionMode(CollisionMode collisionMode);

private:
	// status
	string mName;
	Point2 mOrigin;		/** 原点位置 */
	Rect mBounding;
	int mLayer;
	EntityType mType;
	bool mIsInitialized;
	bool mIsDrawOnYOrder;
	bool mVisibled;
	bool mEnabled;
	bool mBeRemoved;	/** 将要被移除 */
	int mCollisionMode;
	bool mFocused;		/** 是否被设为焦点，应保证最多仅存在一个entity被设为focused*/

	// core member
	LuaContext* mLuaContext;				/** 当前的luaContext */
	Map* mMap;								/** 当前entity所属的map */
	//std::shared_ptr<EntityState> mState;	/** entity 的状态组件 */
	std::unique_ptr<EntityState> mState;
	std::vector<std::unique_ptr<EntityState>> mOldState;

	std::shared_ptr<Movement> mMovement;	/** entity 的运动组件 */

	Entity* mFacingEntity;
	Entity* mOverlapEntity;

	std::vector<NamedSpritePtr> mSprites;	/** entity 所拥有的用于展示的sprites */
	SpritePtr mDebugSprite;
};

using EntityPtr = std::shared_ptr<Entity>;

#endif