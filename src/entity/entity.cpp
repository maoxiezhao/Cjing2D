#include "entity.h"
#include "game\map.h"
#include "game\gameCommands.h"
#include "lua\luaContext.h"
#include "entity\entities.h"
#include "entity\entityState.h"
#include "entity\camera.h"
#include "entity\enemy.h"
#include "movements\movement.h"

Entity::Entity():
	mName(""),
	mOrigin(0, 0),
	mLayer(0),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mFacingEntity(nullptr),
	mOverlapEntity(nullptr),
	mMap(nullptr),
	mIsInitialized(false),
	mIsDrawOnYOrder(false),
	mVisibled(true),
	mDebugSprite(nullptr),
	mDebugSprite1(nullptr),
	mEnabled(true),
	mCollisionMode(COLLISION_NONE),
	mBeRemoved(false),
	mInsertQuadTree(false),
	mFocused(false),
	mCanPushed(false),
	mNotifyScriptMovement(true)
{
}

Entity::Entity(const string & name, const string& templName, const Point2 & pos, const Size & size, int layer):
	mName(name),
	mTemplName(templName),
	mBoundBox{},
	mOrigin(),
	mLayer(layer),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mFacingEntity(nullptr),
	mOverlapEntity(nullptr),
	mMap(nullptr),
	mIsInitialized(false),
	mIsDrawOnYOrder(false),
	mVisibled(true),
	mDebugSprite(nullptr),
	mDebugSprite1(nullptr),
	mEnabled(true),
	mCollisionMode(COLLISION_NONE),
	mBeRemoved(false),
	mInsertQuadTree(false),
	mFocused(false),
	mCanPushed(false),
	mNotifyScriptMovement(true)
{
	mBoundBox[0] = BoundingBox(pos, { size.width + 4, size.height + 4});
	mBoundBox[1] = BoundingBox(pos, { size.width, size.height});
	SetNotifyOffset({ -2, -2 });
}

Entity::~Entity()
{
	ClearSprites();
	ClearRemovedSprite();
	ClearMovements();
}

/**
*	\brief entity刷新动作
*
*	如果存在状态组件则更新状态组件
*/
void Entity::Update()
{
	// sprite
	for (auto namedSprite : mSprites)
	{
		if (namedSprite.removed)
			continue;
	
		namedSprite.sprite->Update();
	}
	ClearRemovedSprite();

	// movement
	if (mMovement != nullptr)
	{
		mMovement->Update();

		// 如果移动完毕，则移除movement
		if (mMovement != nullptr && mMovement->IsFinished())
			StopMovement();
	}

	// state
	UpdateState();
}

/**
*	\brief 绘制entity到map
*/
void Entity::Draw()
{
	//DrawDebugBounding();
	for (auto& nameSprite : mSprites)
	{
		auto sprite = nameSprite.sprite;
		if (sprite != nullptr)
		{
			auto pos = sprite->GetPos() + GetPos();
			GetMap().DrawOnMap(*sprite, pos);
		}
	}
}

/**
*	\brief 完成初始化，当notifyMapStart时调用
*/
void Entity::Initalized()
{
	Debug::CheckAssertion(!mIsInitialized, "The entity has already initialized.");
	Debug::CheckAssertion(mMap != nullptr, "The entity's map is null.");
	Debug::CheckAssertion(mMap->IsLoaded(), "The entity's map has not ready.");

	NotifyBeforeCreated();
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnCreated");
	NotifyAfterCreated();

	mIsInitialized = true;
}

void Entity::ClearMovements()
{
	mMovement = nullptr;
}

/**
*	\brief 设置状态为被关注状态
*/
void Entity::SetFocused(bool bFocused)
{
	if (mFocused != bFocused)
	{
		//if (bFocused)
		//{
		//	// notifyGetFocused()
		//}
		//else
		//{
		//	// notifyLoseFocused()
		//}
		mFocused = bFocused;
	}
}

bool Entity::IsFosused() const
{
	return mFocused;
}

const string Entity::GetLuaObjectName() const
{
	return string();
}

void Entity::DrawDebugBounding()
{
	// notify
	if (mDebugSprite == nullptr)
		mDebugSprite = std::make_shared<Sprite>(Color4B(rand() % (255), rand() % (255), rand() % (255), 255), Size(0, 0));

	Rect box = mBoundBox[0].GetRect();
	float angle = Geometry::Degree(GetFacingDegree());
	mDebugSprite->SetPos(box.GetPos());
	mDebugSprite->SetSize(box.GetSize());
	mDebugSprite->SetDeferredDraw(false);
	GetMap().DrawOnMap(*mDebugSprite);


	// obstacle
	if (mDebugSprite1 == nullptr)
		mDebugSprite1 = std::make_shared<Sprite>(Color4B(rand() % (255), rand() % (255), rand() % (255), 255), Size(0, 0));

	box = mBoundBox[1].GetRect();
	angle = Geometry::Degree(GetFacingDegree());
	mDebugSprite1->SetPos(box.GetPos());
	mDebugSprite1->SetSize(box.GetSize());
	mDebugSprite1->SetDeferredDraw(false);
	GetMap().DrawOnMap(*mDebugSprite1);
}

void Entity::NotifyBeforeCreated()
{
}

void Entity::NotifyAfterCreated()
{
}

void Entity::NotifyCommandPressed(const GameCommand & command)
{
	if (mState != nullptr)
	{
		mState->NotifyCommandPressed(command);
	}
}

void Entity::NotifyCommandReleased(const GameCommand & command)
{
	if (mState != nullptr)
	{
		mState->NotifyCommandReleased(command);
	}
}

void Entity::NotifyMovementChanged()
{
	if (mState != nullptr)
	{
		mState->NotifyMovementChanged();
	}
	if (mNotifyScriptMovement)
	{
		GetLuaContext()->NotifyEntityWithMovement(*this, "OnMovementChanged");
	}
}

/**
*	\brief 响应位置变动
*/
void Entity::NotifyPositonChanged()
{
	NotifyBoundingRectChange();

	// 当本身存在碰撞时，需要告知其他entity对该entity
	if (IsHaveCollision())
		CheckCollisionFromEntities();

	CheckCollisionWithEntities();
}

/**
*	\brief 响应碰撞函数
*	\param otherEntity 发生碰撞的其他实体
*	\param collisonMode 发生的碰撞类型
*/
void Entity::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
}

void Entity::NotifyCollisionWithEnemy(Enemy & enemy)
{
}

void Entity::NotifyCollisionWithPlayer(Player & player)
{
}

void Entity::NotifySpriteCollision(Entity & otherEntity, Sprite & srcSprite, Sprite & otherSprite)
{
}

void Entity::NotifySpriteCollisionWithEnemy(Enemy & enemy, Sprite & srcSprite, Sprite & otherSprite)
{
}

void Entity::NotifySpriteCollisionWithPlayer(Player & player, Sprite & srcSprite, Sprite & otherSprite)
{
}

/**
*	\brief 响应被销毁
*/
void Entity::NotifyBeRemoved()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnRemoved");
	mBeRemoved = true;
}

/**
*	\brief 响应facingEntity的改变
*/
void Entity::NotifyFacingEntityChanged(Entity * entity)
{
	// do nothing
}

/**
*	\brief 响应overlapEntity的改变
*/
void Entity::NotifyOverlapEntityChanged(Entity * entity)
{
	// do nothing
}

void Entity::NotifyDirectionChange(Direction4 oldDir, Direction4 newDir)
{
}

void Entity::NotifyPathFindingFinished()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnPathFindingFinished");
}

/**
*	\brief 当entity的包围矩阵改变时，需要entities改变四叉树的分布
*/
void Entity::NotifyBoundingRectChange()
{
	if (IsOnMap())
		GetMap().GetEntities().NotifyEntityRectChanged(*this);
}

/**
*	\brief 当entity碰到障碍物时响应
*/
void Entity::NotifyObstacleReached()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyObstacleReached");
}

/**
*	\brief 当entity攻击Enemy时响应
*/
void Entity::NotifyAttackEnemy(Enemy & enemy, EntityAttack attack, EntityReactionType reaction)
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyAttackEnemy", [&](lua_State*l) {
		lua_pushinteger(l, static_cast<int>(attack));
		lua_pushinteger(l, static_cast<int>(reaction));
		return 2;
	});
}

/**
*	\brief 当entityg攻击Player时响应
*/
void Entity::NotifyAttackPlayer(Player & player, EntityAttack attack, EntityReactionType reaction)
{
}

/**
*	\brief 响应交互键的按下
*	\param interactEntity 发起交互的实体
*	\return true当交互成功发生，false改实体无法交互
*
*	对于道具、NPC会在该函数中做出响应	
*/
bool Entity::NotifyCommandInteractPressed(Entity& interactEntity)
{
	return false;
}

/**
*	\brief 当地图完成加载后执行完成初始化
*/
void Entity::NotifyMapStarted()
{
	if (!mIsInitialized)
	{
		Initalized();
	}
}

/**
*	\biref 设置当前地图
*
*	所有的entity在setmap之后才算完成全部初始化
*/
void Entity::SetMap(Map * map)
{
	mMap = map;
	SetLuaContext(&map->GetLuaContext());

	if (!mIsInitialized && mMap->IsLoaded())
	{	// 可能在游戏中实时创建entity
		// 则在加入地图时初始化
		Initalized();
	}
}

Map & Entity::GetMap()
{
	return *mMap;
}

/**
*	\brief 是否在地图中
*/
bool Entity::IsOnMap() const
{
	return mMap != nullptr;
}

const Map & Entity::GetMap() const
{
	return *mMap;
}

/**
*	\brief 将该enitty从设置的地图中销毁
*/
void Entity::RemoveFromMap()
{
	auto& entities = mMap->GetEntities();
	entities.RemoveEntity(*this);
}

Game & Entity::GetGame()
{
	Debug::CheckAssertion(mMap != nullptr,
		"The invalid entity withou map.");
	return mMap->GetGame();
}

/**
*	\brief 创建sprite,并设置sprite
*/
SpritePtr Entity::CreateSprite(const string & spriteName)
{
	// sprite
	SpritePtr sprite = std::make_shared<Sprite>(spriteName);

	NamedSpritePtr namedSprite;
	namedSprite.name = spriteName;
	namedSprite.sprite = sprite;
	namedSprite.removed = false;

	mSprites.push_back(namedSprite);
	NotifyBoundingRectChange();

	return sprite;
}

/**
*	\brief 创建animaiton,并设置sprite
*/
AnimationSpritePtr Entity::CreateAnimationSprite(const string & animationSetId, const string & animationID)
{
	// animationSprite
	AnimationSpritePtr animationSprite = std::make_shared<AnimationSprite>(animationSetId);
	if(animationID != "")
		animationSprite->SetCurrAnimation(animationID);
	//animationSprite->SetPos(GetPos());
	animationSprite->SetDeferredDraw(true);
	NamedSpritePtr namedSprite;
	namedSprite.name = animationID;
	namedSprite.sprite = animationSprite;
	namedSprite.removed = false;

	mSprites.push_back(namedSprite);
	NotifyBoundingRectChange();

	return animationSprite;
}

SpritePtr Entity::GetSprite(const string & spriteName)
{
	for (auto sprite : mSprites)
	{
		if (sprite.name == spriteName &&
			sprite.removed == false)
		{
			return sprite.sprite;
		}
	}
	return nullptr;
}

bool Entity::RemoveSprite(SpritePtr sprite)
{
	for (auto& namedSprite : mSprites)
	{
		if (namedSprite.sprite == sprite &&
			namedSprite.removed == false)
		{
			namedSprite.removed = true;
			return true;
		}
	}
	return false;
}

bool Entity::RemoveSprite(const std::string & spriteName)
{
	bool result = false;
	auto sprite = GetSprite(spriteName);
	if (sprite)
		result = RemoveSprite(sprite);
	return result;
}

/**
*	\brief 删除所有sprite
*
*	真正的清除操作在所有的sprite update之后调用
*	clearRemovedSprites执行
*/
void Entity::ClearSprites()
{
	for (auto& sprite : mSprites)
	{
		sprite.removed = true;
	}
}

/**
*	\brief 释放所有被标记为removed的sprite
*/
void Entity::ClearRemovedSprite()
{
	for (auto it = mSprites.begin(); it != mSprites.end();)
	{
		if (it->removed)
		{
			it = mSprites.erase(it);
		}
		else
		{
			++it;
		}

	}
}

std::vector<Entity::NamedSpritePtr>& Entity::GetSprites()
{
	return mSprites;
}

EntityState& Entity::GetState()const
{
	return *(mState.get());
}

void Entity::SetState(EntityState * state)
{
	auto oldState = mState.get();
	if (oldState)
	{
		oldState->Stop(*state);
		if (oldState != mState.get())
		{
			// 可能在oldState::Stop中非法改变了当前的state
			Debug::Error("Invalid Change Entity State:" + oldState->GetName());
			return;
		}
	}
	// 这里做一个延迟删除，一帧可能会被调用到
	mOldState.emplace_back(std::move(mState));

	mState = std::unique_ptr<EntityState>(state);
	mState->Start(*oldState);
}

void Entity::UpdateState()
{
	if (mState != nullptr)
	{
		mState->Update();
	}
	mOldState.clear();
}

void Entity::StopMovement()
{
	if (mMovement)
	{
		mMovement->SetEntity(nullptr);
		mMovement->Stop();
	}
	mMovement = nullptr;
}

void Entity::StartMovement(const std::shared_ptr<Movement>& movement)
{
	StopMovement();
	mMovement = movement;
	mMovement->SetEntity(this);
	mMovement->Start();
}

const std::shared_ptr<Movement>& Entity::GetMovement()
{
	return mMovement;
}

const std::shared_ptr<Movement>& Entity::GetMovement() const
{
	return mMovement;
}

/**
*	\brief 检测该实例与其他实例之间是否发生碰撞
*/
void Entity::CheckCollisionWithEntities()
{
	if (!IsOnMap())
		return;

	// 碰撞检测
	GetMap().CheckCollisionWithEntities(*this);

	// 像素级碰撞检测
	auto& nameSprites = GetSprites();
	for (const auto& nameSprite : nameSprites)
	{
		if(nameSprite.removed)
			continue;

		auto& sprite = *nameSprite.sprite;
		if (sprite.IsPixelCollisionEnable())
			GetMap().CheckCollisionWithEntities(*this, sprite);
	}
}

void Entity::CheckCollisionFromEntities()
{
	if (!IsOnMap())
		return;
	GetMap().CheckCollisionFromEntities(*this);

	// 像素级碰撞检测
	auto& nameSprites = GetSprites();
	for (const auto& nameSprite : nameSprites)
	{
		if (nameSprite.removed)
			continue;

		auto& sprite = *nameSprite.sprite;
		if (sprite.IsPixelCollisionEnable())
			GetMap().CheckCollisionFromEntities(*this, sprite);
	}
}

/**
*	\brief 检测实体间的碰撞检测
*	\param otherEntity 待检测的其他实体
*
*	当发生碰撞检测后，根据碰撞类型调用notifyCollision,
*	响应触发事件
*/
void Entity::CheckCollision(Entity & otherEntity)
{
	if (!IsHaveCollision() || this == &otherEntity)
	{
		return;
	}
	// 层级需要相同,且设定为同层级碰撞
	if (GetLayer() != otherEntity.GetLayer())
	{
		return;
	}
	// 根据当前的碰撞模式测试碰撞
	if (HasCollisionMode(CollisionMode::COLLISION_OVERLAPING) &&
		TestCollisionOverlaping(otherEntity))
	{
		if (otherEntity.GetOverlapEntity() == nullptr)
		{
			otherEntity.SetOverlapEntity(this);
		}
		NotifyCollision(otherEntity, CollisionMode::COLLISION_OVERLAPING);
	}
	else if (HasCollisionMode(CollisionMode::COLLISION_CONTAINING) &&
		TestCollisionContaining(otherEntity))
	{
		NotifyCollision(otherEntity, CollisionMode::COLLISION_CONTAINING);
	}
}

/**
*	\brief 检测精灵间像素级碰撞检测
*/
void Entity::CheckSpriteCollision(Entity & otherEntity, Sprite & otherSprite)
{
	if (HasCollisionMode(COLLLSION_SPRITE))
	{
		if (&otherEntity == this ||
			otherEntity.GetLayer() != GetLayer())
			return;

		auto& nameSprites = GetSprites();
		for (const auto& nameSprite : nameSprites)
		{
			auto&sprite = nameSprite.sprite;
			if (sprite->IsPixelCollisionEnable())
			{
				if (sprite->TestCollision(otherSprite, GetPos(), otherEntity.GetPos()))
					NotifySpriteCollision(otherEntity, *sprite, otherSprite);
			}
		}
	}
}

/**
*	\brief 当collisonMode不等于collision_none时参与碰撞
*/
bool Entity::IsHaveCollision() const
{
	return mCollisionMode != COLLISION_NONE;
}

/**
*	\brief 测试实体与其他实体间矩形碰撞
*/
bool Entity::TestCollisionOverlaping(const Entity & entity, int type)
{
	BoundingBox box = entity.GetBoundingBox(type);
	return TestCollisionWithBox(box);
}

bool Entity::TestCollisionWithBox(const BoundingBox box, int type)
{
	return GetBoundingBox(type).Overlaps(box);
}

bool Entity::TestCollisionWithRect(const Rect & rect, int type)
{
	return GetRectBounding(type).Overlaps(rect);
}

bool Entity::TestCollisionContaining(const Entity & entity, int type)
{
	return false;
}

/**
*	\brief 设置当前碰撞模式
*/
void Entity::SetCollisionMode(CollisionMode collisionMode)
{
	mCollisionMode |= collisionMode;
}

/**
*	\brief 返回当前是否存在该碰撞模式
*/
bool Entity::HasCollisionMode(CollisionMode collisionMode)
{
	return ((collisionMode & mCollisionMode) != 0);
}

Rect Entity::GetRectBounding(int type) const
{
	return mBoundBox[type].GetRect();
}

void Entity::SetNotifySize(const Size & size)
{
	mBoundBox[BOUNDING_BOX_NOTIFY].SetSize(size);
}

Size Entity::GetNotifySize() const
{
	return mBoundBox[BOUNDING_BOX_NOTIFY].GetSize();
}

void Entity::SetBoundingAngle(float angle)
{
	for(auto& box : mBoundBox)
		box.SetRotate(angle);
}

float Entity::GetBoundingAngle() const
{
	return mBoundBox[0].GetRotate();
}

void Entity::SetDrawOnYOrder(bool isDrawOnY)
{
	mIsDrawOnYOrder = isDrawOnY;
}

bool Entity::IsDrawOnYOrder() const
{
	return mIsDrawOnYOrder;
}

void Entity::SetOrigin(const Point2 & origin)
{
	for (auto& box : mBoundBox)
		box.AddPos(mOrigin.x - origin.x, mOrigin.y - origin.y);

	mOrigin = origin;
}

const Point2 & Entity::GetOrigin() const
{
	return mOrigin;
}

/**
*	\brief 返回屏幕坐标
*/
Point2 Entity::GetScreenPos() const
{
	auto camPos = GetMap().GetCamera()->GetLeftTopPos();
	return GetPos() - camPos;
}

bool Entity::IsVisible()const
{
	return mVisibled;
}

void Entity::SetVisible(bool visibled)
{
	mVisibled = visibled;
}

bool Entity::IsBeRemoved() const
{
	return mBeRemoved;
}

bool Entity::IsInserQuadTree() const
{
	return mInsertQuadTree;
}

void Entity::SetInsertQuadTree(bool inserted) 
{
	mInsertQuadTree = inserted;
}

void Entity::SetSuspended(bool suspended)
{
}

bool Entity::IsSuspended() const
{
	return false;
}

void Entity::SetEnable(bool enable)
{
	mEnabled = enable;
}

bool Entity::IsEnable() const
{
	return mEnabled;
}

Point2 Entity::GetPos()const
{
	return mBoundBox[BOUNDING_BOX_OBSTACLE].GetPos() + mOrigin;
}
Point2 Entity::GetCenterPos() const
{
	Size size = GetSize();
	Point2 pos = mBoundBox[BOUNDING_BOX_OBSTACLE].GetPos();
	return Point2(pos.x + size.width / 2,
		pos.y + size.height / 2);
}

Point2 Entity::GetLeftTopPos() const
{
	return mBoundBox[BOUNDING_BOX_OBSTACLE].GetPos();
}

Point2 Entity::GetAttachPos() const
{
	Size size = GetSize();
	return{ (int)(size.width * 0.6), size.height / 2 };
}

void Entity::SetPos(const Point2& pos)
{
	int x = pos.x - mOrigin.x, y = pos.y - mOrigin.y;
	mBoundBox[BOUNDING_BOX_OBSTACLE].SetPos(x, y);
	mBoundBox[BOUNDING_BOX_NOTIFY].SetPos(x + mNotifyOffset.x, y + mNotifyOffset.y);
	NotifyBoundingRectChange();
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

void Entity::SetTemplName(const string & name)
{
	mTemplName = name;
}

void Entity::SetSize(const Size & size)
{
	mBoundBox[BOUNDING_BOX_NOTIFY].SetSize(size.width, size.height);
	mBoundBox[BOUNDING_BOX_OBSTACLE].SetSize(size.width, size.height);
	NotifyBoundingRectChange();
}

Size Entity::GetSize() const
{
	return mBoundBox[BOUNDING_BOX_OBSTACLE].GetSize();
}

void Entity::SetNotifyOffset(const Point2 & offset)
{
	mNotifyOffset = offset;
	Point2 pos = GetPos();
	mBoundBox[BOUNDING_BOX_NOTIFY].SetPos(pos.x + mNotifyOffset.x, pos.y + mNotifyOffset.y);
}

string Entity::GetName()const
{
	return mName;
}

string Entity::GetTemplName() const
{
	return mTemplName;
}

BoundingBox Entity::GetBoundingBox(int type) const
{
	Debug::CheckAssertion(type >= 0 && type <= 1, "Invalid bounding box type.");
	return mBoundBox[type];
}

EntityType Entity::GetEntityType()const
{
	return mType;
}

/**
*	\brief 设置entity的注意方向
*/
void Entity::SetFacingDegree(float degree)
{
	auto& sprites = GetSprites();
	for (auto& namesprite : sprites)
	{
		if (!namesprite.removed)
		{
			auto& sprite = namesprite.sprite;
			degree = Geometry::Degree(degree);
			sprite->SetRotated(degree);
		}
	}
}

/**
*	\brief 返回entity注意的方向
*/
float Entity::GetFacingDegree() const
{
	return 0.0f;
}

void Entity::SetDirection(Direction4 dir)
{
	if (dir != mDirection)
	{
		auto oldDir = dir;
		mDirection = dir;
		NotifyDirectionChange(oldDir, dir);
	}
}

Direction4 Entity::GetDirection() const
{
	return mDirection;
}

/**
*	\brief 是否可以推动，目前仅player可以对其他entity进行推动
*/
bool Entity::CanPushed() const
{
	return mCanPushed;
}

void Entity::SetCanPushed(bool pushed)
{
	mCanPushed = pushed;
}

void Entity::StartMoveByPushed(Entity & entity)
{
}

bool Entity::IsObstacle(Entity & entity) const
{
	return false;
}

bool Entity::IsObstacleEnemy() const
{
	return false;
}

bool Entity::IsObstaclePlayer() const
{
	return false;
}

void Entity::SetFacingEntity(Entity * entity)
{
	NotifyFacingEntityChanged(entity);
	mFacingEntity = entity;
}

Entity * Entity::GetFacingEntity()
{
	return mFacingEntity;
}

void Entity::SetOverlapEntity(Entity * entity)
{
	NotifyOverlapEntityChanged(entity);
	mOverlapEntity = entity;
}

Entity * Entity::GetOverlapEntity()
{
	return mOverlapEntity;
}

