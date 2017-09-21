#include "entity.h"
#include "game\map.h"
#include "lua\luaContext.h"
#include "entity\entityState.h"

Entity::Entity():
	mName(""),
	mPos(),
	mSize(),
	mLayer(0),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mIsInitialized(false)
{
}

Entity::Entity(const string & name, const Point2 & pos, const Size & size, int layer):
	mName(name),
	mPos(pos),
	mSize(size),
	mLayer(layer),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mIsInitialized(false)
{
}

/**
*	\brief entity刷新动作
*
*	如果存在状态组件则更新状态组件
*/
void Entity::Update()
{

	UpdateState();
}

void Entity::Draw()
{
}

/**
*	\brief 完成初始化，当notifyMapStart时调用
*/
void Entity::Initalized()
{
	Debug::CheckAssertion(!mIsInitialized, "The entity has already initialized.");
	Debug::CheckAssertion(mMap != nullptr, "The entity's map is null.");
	Debug::CheckAssertion(mMap->IsLoaded(), "The entity's map has not ready.");

	mIsInitialized = true;
}

void Entity::ClearMovements()
{
}

const string Entity::GetLuaObjectName() const
{
	return string();
}

void Entity::NotifyCommandPressed(const GameCommand & command)
{
	// do nothing
}

void Entity::NotifyCommandReleased(const GameCommand & command)
{
	// do nothing
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
}

Map & Entity::GetMap()
{
	return *mMap;
}

const std::shared_ptr<EntityState>& Entity::GetState()
{
	return mState;
}

void Entity::SetState(const std::shared_ptr<EntityState>& state)
{

}

void Entity::UpdateState()
{
}

void Entity::StopMovement()
{
}

void Entity::StartMovement(const std::shared_ptr<Movement>& movement)
{
}

const std::shared_ptr<Movement>& Entity::GetMovement()
{
	return mMovement;
}

Rect Entity::GetRectBounding() const
{
	return Rect(mPos, mSize);
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