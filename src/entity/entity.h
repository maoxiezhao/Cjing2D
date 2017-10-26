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
*	\brief ��Ϸʵ��ĳ�����
*/
class Entity : public LuaObject
{
public:
	/**
	*	���������Ե�share_ptr<sprite>
	*/
	struct NamedSpritePtr
	{
		std::string name;
		SpritePtr sprite;
		bool removed;
	};

	/**
	*	\brief ��ײģʽ
	*/
	enum CollisionMode
	{
		COLLISION_NONE      = 0x00,     // ����ײģʽ����������ײ
		COLLISION_OVERLAPING = 0x01,	// ����ģʽ
		COLLISION_CONTAINING = 0x02,	// ����ģʽ
	};

	Entity();
	Entity(const string& name, const Point2& pos, const Size& size, int layer);
	~Entity();

	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

	void ClearMovements();
	virtual const string GetLuaObjectName()const;

	void DrawDebugBounding();

	/**** **** **** Notify **** **** ****/
	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);
	virtual void NotifyMovementChanged();
	virtual void NotifyPositonChanged();
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual void NotifyBeRemoved();

	void NotifyMapStarted();
	
public:
	/**** **** **** status **** **** ****/
	Point2 GetPos()const;
	Point2 GetCenterPos()const;
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
	const std::shared_ptr<EntityState>& GetState();
	void SetState(const std::shared_ptr<EntityState>& state);
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
	Point2 mOrigin;		/** ԭ��λ�� */
	Rect mBounding;
	int mLayer;
	EntityType mType;
	bool mIsInitialized;
	bool mIsDrawOnYOrder;
	bool mVisibled;
	bool mEnabled;
	bool mBeRemoved;	/** ��Ҫ���Ƴ� */
	int mCollisionMode;

	// core member
	LuaContext* mLuaContext;				/** ��ǰ��luaContext */
	Map* mMap;								/** ��ǰentity������map */
	std::shared_ptr<EntityState> mState;	/** entity ��״̬��� */
	std::shared_ptr<Movement> mMovement;	/** entity ���˶���� */

	std::vector<NamedSpritePtr> mSprites;	/** entity ��ӵ�е�����չʾ��sprites */
	SpritePtr mDebugSprite;
};

using EntityPtr = std::shared_ptr<Entity>;

#endif