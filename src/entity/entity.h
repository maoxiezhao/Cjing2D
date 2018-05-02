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
class Enemy;

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
	Entity(const string& name, const string& templName, const Point2& pos, const Size& size, int layer);
	~Entity();

	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

	void DrawDebugBounding();
	void ClearMovements();

	/** Lua Context */
	virtual const string GetLuaObjectName()const;

	/**** **** **** Notify **** **** ****/
	virtual void NotifyBeforeCreated();
	virtual void NotifyAfterCreated();
	virtual void NotifyCommandPressed(const GameCommand& command);
	virtual void NotifyCommandReleased(const GameCommand& command);
	virtual void NotifyMovementChanged();
	virtual void NotifyPositonChanged();
	virtual void NotifyBeRemoved();
	virtual void NotifyFacingEntityChanged(Entity* entity);
	virtual void NotifyOverlapEntityChanged(Entity* entity);
	virtual void NotifyDirectionChange(Direction4 oldDir, Direction4 newDir);
	virtual void NotifyPathFindingFinished();
	virtual void NotifyBoundingRectChange();

	// notify colision
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual void NotifyCollisionWithEnemy(Enemy& enemy);

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
	void SetTemplName(const string& name);
	void SetSize(const Size& size);
	Size GetSize()const;
	string GetName()const;
	string GetTemplName()const;
	Rect GetRectBounding()const;
	void SetDrawOnYOrder(bool isDrawOnY);
	bool IsDrawOnYOrder()const;
	void SetOrigin(const Point2& origin);
	const Point2& GetOrigin()const;
	Point2 GetScreenPos()const;
	bool IsVisible()const;
	void SetVisible(bool visibled);
	bool IsBeRemoved()const;
	bool IsInserQuadTree()const;
	void SetInsertQuadTree(bool inserted);
	virtual void SetSuspended(bool suspended);
	bool IsSuspended()const;
	virtual EntityType GetEntityType()const;
	virtual float GetFacingDegree()const;
	virtual void SetDirection(Direction4 dir);
	Direction4 GetDirection()const;
	bool CanPushed()const;
	void SetCanPushed(bool pushed);
	virtual void StartMoveByPushed(Entity& entity);

	/** obstacle */
	virtual bool IsObstacle(Entity& entity)const;
	virtual bool IsObstacleEnemy()const;
	virtual bool IsObstaclePlayer()const;

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
	AnimationSpritePtr CreateAnimationSprite(const string & animationSetId, const string & animationID = "");
	SpritePtr GetSprite(const string& spriteName);
	bool RemoveSprite(SpritePtr sprite);
	bool RemoveSprite(const std::string& spriteName);
	void ClearSprites();
	void ClearRemovedSprite();
	std::vector<NamedSpritePtr>& GetSprites();

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
	void CheckCollisionFromEntities();
	virtual void CheckCollision(Entity& otherEntity);

	bool TestCollisionWithRect(const Entity& entity);
	bool TestCollisionWithRect(const Rect& rect);
	bool TestCollisionContaining(const Entity& entity);

	bool IsHaveCollision()const;
	void SetCollisionMode(CollisionMode collisionMode);
	bool HasCollisionMode(CollisionMode collisionMode);

private:
	// status
	string mName;		/** �Զ������֣�������ͬ */
	string mTemplName;  /** ģ������ */
	Point2 mOrigin;		/** ԭ��λ�� */
	Rect mBounding;
	int mLayer;
	EntityType mType;
	bool mIsInitialized;
	bool mIsDrawOnYOrder;
	bool mVisibled;
	bool mEnabled;
	bool mBeRemoved;	 /** ��Ҫ���Ƴ� */
	bool mInsertQuadTree;/** �Ƿ���뵽�Ĳ����� */
	int mCollisionMode;
	bool mFocused;		 /** �Ƿ���Ϊ���㣬Ӧ��֤��������һ��entity����Ϊfocused*/
	Direction4 mDirection;
	bool mCanPushed;
	bool mNotifyScriptMovement;

	// core member
	LuaContext* mLuaContext;				/** ��ǰ��luaContext */
	Map* mMap;								/** ��ǰentity������map */
	//std::shared_ptr<EntityState> mState;	/** entity ��״̬��� */
	std::unique_ptr<EntityState> mState;
	std::vector<std::unique_ptr<EntityState>> mOldState;

	std::shared_ptr<Movement> mMovement;	/** entity ���˶���� */

	Entity* mFacingEntity;
	Entity* mOverlapEntity;

	std::vector<NamedSpritePtr> mSprites;	/** entity ��ӵ�е�����չʾ��sprites */
	SpritePtr mDebugSprite;
};

using EntityPtr = std::shared_ptr<Entity>;

#endif