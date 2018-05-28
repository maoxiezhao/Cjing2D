#ifndef _ENTITY_H_
#define _ENTITY_H_

#include"common\common.h"
#include"lua\luaObject.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"
#include"utils\boundingBox.h"
#include"entity\entityInfo.h"
#include"entity\entityAttack.h"
#include"game\sprite.h"
#include"game\gameCommands.h"
#include"game\animationSprite.h"

class Sprtie;
class LuaContext;
class Movement;
class Map;
class EntityState;
class Enemy;
class Player;
class Block;
class Chest;

/**
*	\brief 游戏实体的抽象类
*
*	2018.5.9
*	做了个碰撞盒相关处理，分为障碍碰撞盒和响应范围盒，响应范围盒用于处理Notify事件
*	而障碍碰撞和则用于处理位置和障碍物碰撞，和原先Rect一致
*
*	2018.5.20
*	添加AttachEntity，AttachEntity全部由被挂载的Entity管理，自动添加到地图和从地图中
*	移除
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
		COLLISION_NONE = 0x00,          // 无碰撞模式，不参与碰撞
		COLLISION_OVERLAPING = 0x01,	// 覆盖模式
		COLLISION_CONTAINING = 0x02,	// 包含模式
		COLLLSION_SPRITE = 0x04,        // 精灵间像素碰撞检测
		COLLISION_FACING = 0x08,
	};

	/**
	*	\brief BoundingBoxType
	*/
	enum BoundingBoxType
	{
		BOUNDING_BOX_NOTIFY = 0,
		BOUNDING_BOX_OBSTACLE,
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
	virtual void NotifyMovementFinished();
	virtual void NotifyPositonChanged();
	virtual void NotifyBeRemoved();
	virtual void NotifyFacingEntityChanged(Entity* entity);
	virtual void NotifyOverlapEntityChanged(Entity* entity);
	virtual void NotifyDirectionChange(Direction4 oldDir, Direction4 newDir);
	virtual void NotifyPathFindingFinished();
	virtual void NotifyBoundingRectChange();
	virtual void NotifyObstacleReached();
	virtual void NotifyAttackEnemy(Enemy& enemy,EntityAttack attack, EntityReactionType reaction);
	virtual void NotifyAttackPlayer(Player& player, EntityAttack attack, EntityReactionType reaction);
	virtual void NotifyFocusChange(bool focused);

	// game command notify
	virtual bool NotifyCommandInteractPressed(Entity& interactEntity);

	void NotifyMapStarted();
	
public:
	/**** **** **** status **** **** ****/
	virtual EntityType GetEntityType()const;

	/** base prop */
	Point2 GetPos()const;
	Point2 GetCenterPos()const;
	Point2 GetLeftTopPos()const;
	Point2 GetAttachPos()const;
	Point2 GetRotatePos()const;
	Point2 GetRotateDiffPos()const;
	Point2 GetOrigin()const;
	Point2 GetScreenPos()const;
	Point2 GetPositivePos()const;

	virtual Point2 GetBillBoardPos()const;
	void SetPos(const Point2& pos);
	void SetLayer(int layer);
	int GetLayer()const;
	void SetName(const string& name);
	void SetTemplName(const string& name);
	string GetName()const;
	string GetTemplName()const;
	void SetBoundingAngle(float angle);
	float GetBoundingAngle()const;
	void SetDrawOnYOrder(bool isDrawOnY);
	bool IsDrawOnYOrder()const;
	void SetOrigin(const Point2& origin);

	bool IsVisible()const;
	void SetVisible(bool visibled);
	bool IsBeRemoved()const;
	bool IsInserQuadTree()const;
	void SetInsertQuadTree(bool inserted);
	virtual void SetSuspended(bool suspended);
	bool IsSuspended()const;
	void SetEnable(bool enable);
	bool IsEnable()const;

	BoundingBox GetBoundingBox(int type)const;
	Rect GetRectBounding(int type = BOUNDING_BOX_NOTIFY)const;
	void SetSize(const Size& size);
	Size GetSize()const;

	void SetNotifySize(const Size& size);
	Size GetNotifySize()const;
	void SetNotifyOffset(const Point2& offset);

	/** direction */
	virtual void SetFacingDegree(float degree);
	virtual float GetFacingDegree()const;
	virtual Point2 GetFacingPoint()const;
	Point2 GetPointByDirection(Direction4 direction)const;
	virtual void SetBoundRotateAnchor(float x, float y);
	virtual void SetDirection(Direction4 dir);
	Direction4 GetDirection()const;
	
	bool CanPushed()const;
	void SetCanPushed(bool pushed);
	
	/** obstacle */
	virtual bool IsObstacle(Entity& entity)const;
	virtual bool IsObstacleEnemy()const;
	virtual bool IsObstaclePlayer()const;
	virtual bool IsObstacleBlock()const;
	virtual bool isObstacleChest()const;

	// special status
	void SetFacingEntity(Entity* entity);
	Entity* GetFacingEntity();
	void SetOverlapEntity(Entity* entity);
	Entity* GetOverlapEntity();
	void SetFocused(bool bFocused);
	bool IsFosused()const;
	void AddAttachEntity(std::shared_ptr<Entity> attachEntity, const Point2& offset);
	void RemoveAttachEntity(std::shared_ptr<Entity> attachEntity);
	Point2 GetAttachOffset()const;
	void SetAttachOffset(const Point2& offset);
	void UpdateAttachEntities();
	bool HasAttachEntity(Entity& checkEntity)const;

	void SetMap(Map* map);
	Map& GetMap();
	bool IsOnMap()const;
	const Map& GetMap()const;
	void RemoveFromMap();
	Game& GetGame();

	/**** ***** *** sprite *** ***** ****/
	SpritePtr CreateSprite(const string & spriteName);
	AnimationSpritePtr CreateAnimationSprite(const string & animationSetId, const string & animationID = "");
	SpritePtr GetSprite(const string& spriteName);
	SpritePtr GetFirstSprite();
	bool RemoveSprite(SpritePtr sprite);
	bool RemoveSprite(const std::string& spriteName);
	void ClearSprites();
	void ClearRemovedSprite();
	std::vector<NamedSpritePtr>& GetSprites();

	/**** ***** *** state manager *** ***** ****/
	EntityState& GetState()const;
	void SetState(EntityState* state);
	void UpdateState();

	/***** **** *** movement manager *** ***** *****/
	void StopMovement();
	void StartMovement(const std::shared_ptr<Movement>& movement);
	const std::shared_ptr<Movement>& GetMovement();
	const std::shared_ptr<Movement>& GetMovement()const;
	virtual bool StartMoveByPushed(Entity& entity);
	virtual bool StopMoveByPushed();

	/***** **** *** collision *** ***** *****/
	void CheckCollisionWithEntities();
	void CheckCollisionFromEntities();

	bool TestCollisionContaining(const Entity& entity, int type = BOUNDING_BOX_NOTIFY);
	bool TestCollisionOverlaping(const Entity& entity, int type = BOUNDING_BOX_NOTIFY);
	bool TestCollisionWithBox(const BoundingBox box,   int type = BOUNDING_BOX_NOTIFY);
	bool TestCollisionWithRect(const Rect& rect,       int type = BOUNDING_BOX_NOTIFY);
	bool TestCollisionFacing(const Entity& entity,     int type = BOUNDING_BOX_NOTIFY);

	bool IsHaveCollision()const;
	void SetCollisionMode(CollisionMode collisionMode);
	bool HasCollisionMode(CollisionMode collisionMode);

	virtual void CheckCollision(Entity& otherEntity);
	virtual void CheckSpriteCollision(Entity& otherEntity, Sprite& otherSprite);

	// notify collision
	virtual void NotifyCollision(Entity& otherEntity, CollisionMode collisionMode);
	virtual void NotifyCollisionWithEnemy(Enemy& enemy);
	virtual void NotifyCollisionWithPlayer(Player& player);
	virtual void NotifyCollisionWithBlock(Block& block);
	virtual void NotifyCollisionWithChest(Chest& chest);

	// notify pixel collision
	virtual void NotifySpriteCollision(Entity& otherEntity, Sprite& srcSprite, Sprite& otherSprite);
	virtual void NotifySpriteCollisionWithEnemy(Enemy& enemy, Sprite& srcSprite, Sprite& otherSprite);
	virtual void NotifySpriteCollisionWithPlayer(Player& player, Sprite& srcSprite, Sprite& otherSprite);

	static void SetDebugDrawBounding(bool drawed);
	static bool isDebugDrawBounding;	/** 是否绘制包围盒 */
private:
	// status
	string mName;			  /** 自定义名字，不可相同 */
	string mTemplName;		  /** 模板名字 */
	Point2 mOrigin;		 	  /** 原点位置 */
	Point2 mNotifyOffset;	  /** 响应包围盒的偏移值 */
	BoundingBox mBoundBox[2]; /** 1响应包围盒 2碰撞包围盒 */	
	int mLayer;
	EntityType mType;
	bool mIsInitialized;
	bool mIsDrawOnYOrder;
	bool mVisibled;
	bool mEnabled;
	bool mBeRemoved;		 /** 将要被移除 */
	bool mInsertQuadTree;	 /** 是否插入到四叉树中 */
	int mCollisionMode;
	bool mFocused;			 /** 是否被设为焦点，应保证最多仅存在一个entity被设为focused*/
	Direction4 mDirection;
	bool mCanPushed;
	bool mNotifyScriptMovement;
	bool mStopMovement;		/** 临时用来处理mMovement在结束后为nullptr，有时会被设置
								为非空导致访问无效内存，所以加个flag做强制保护，找到实际原因后删除 */
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
	SpritePtr mDebugSprite1;

	std::vector<std::shared_ptr<Entity> > mAttachEntities;
	Point2 mAttachOffset;
};

using EntityPtr = std::shared_ptr<Entity>;

#endif