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
	mBounding(),
	mOrigin(0, 0),
	mLayer(0),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mFacingEntity(nullptr),
	mOverlapEntity(nullptr),
	mIsInitialized(false),
	mIsDrawOnYOrder(false),
	mVisibled(true),
	mDebugSprite(nullptr),
	mEnabled(true),
	mCollisionMode(COLLISION_NONE),
	mBeRemoved(false),
	mFocused(false)
{
}

Entity::Entity(const string & name, const string& templName, const Point2 & pos, const Size & size, int layer):
	mName(name),
	mTemplName(templName),
	mBounding(pos, size),
	mOrigin(),
	mLayer(layer),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mFacingEntity(nullptr),
	mOverlapEntity(nullptr),
	mIsInitialized(false),
	mIsDrawOnYOrder(false),
	mVisibled(true),
	mDebugSprite(nullptr),
	mEnabled(true),
	mCollisionMode(COLLISION_NONE),
	mBeRemoved(false),
	mFocused(false)
{
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
		{
			continue;
		}
		namedSprite.sprite->Update();
	}
	ClearRemovedSprite();

	// movement
	if (mMovement != nullptr)
	{
		mMovement->Update();
		// 如果移动完毕，则移除movement
		if (mMovement != nullptr && mMovement->IsFinished())
		{
			StopMovement();
		}
	}

	// state
	UpdateState();
}

/**
*	\brief 绘制entity到map
*/
void Entity::Draw()
{
	DrawDebugBounding();
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
	return;
	if (mDebugSprite == nullptr)
	{
		mDebugSprite = std::make_shared<Sprite>(Color4B(rand() % (255), rand() % (255), rand() % (255), 255), Size(0, 0));
	}

	mDebugSprite->SetPos(mBounding.GetPos());
	mDebugSprite->SetSize(mBounding.GetSize());
	mDebugSprite->SetDeferredDraw(false);
	GetMap().DrawOnMap(*mDebugSprite);
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
}

/**
*	\brief 响应位置变动
*/
void Entity::NotifyPositonChanged()
{
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

/**
*	\brief 响应被销毁
*/
void Entity::NotifyBeRemoved()
{
	// luaContext->onRemovedEneity(*this) 响应lua
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
	for (auto namedSprite : mSprites)
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
	for (auto sprite : mSprites)
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

/**
*	\brief 检测该实例与其他实例之间是否发生碰撞
*/
void Entity::CheckCollisionWithEntities()
{
	if (!IsOnMap())
		return;
	GetMap().CheckCollisionWithEntities(*this);
}

void Entity::CheckCollisionFromEntities()
{
	if (!IsOnMap())
		return;
	GetMap().CheckCollisionFromEntities(*this);
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
		TestCollisionWithRect(otherEntity))
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
*	\brief 当collisonMode不等于collision_none时参与碰撞
*/
bool Entity::IsHaveCollision() const
{
	return mCollisionMode != COLLISION_NONE;
}

/**
*	\brief 测试实体与其他实体间矩形碰撞
*/
bool Entity::TestCollisionWithRect(const Entity & entity)
{
	auto otheRect = entity.GetRectBounding();
	return GetRectBounding().Overlaps(otheRect);
}

bool Entity::TestCollisionContaining(const Entity & entity)
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

Rect Entity::GetRectBounding() const
{
	return mBounding;
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
	mBounding.AddPos(mOrigin.x - origin.x, mOrigin.y - origin.y);
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

void Entity::SetSuspended(bool suspended)
{
}

bool Entity::IsSuspended() const
{
	return false;
}

Point2 Entity::GetPos()const
{
	return mBounding.GetPos() + mOrigin;
}
Point2 Entity::GetCenterPos() const
{
	Size size = GetSize();
	Point2 pos = mBounding.GetPos();
	return Point2(pos.x + size.width / 2,
		pos.y + size.height / 2);
}

Point2 Entity::GetLeftTopPos() const
{
	return mBounding.GetPos();
}

void Entity::SetPos(const Point2& pos)
{
	mBounding.SetPos(pos.x - mOrigin.x, pos.y - mOrigin.y);
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
	mBounding.SetSize(size.width, size.height);
}

Size Entity::GetSize() const
{
	return mBounding.GetSize();
}

string Entity::GetName()const
{
	return mName;
}

string Entity::GetTemplName() const
{
	return mTemplName;
}

EntityType Entity::GetEntityType()const
{
	return mType;
}

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

