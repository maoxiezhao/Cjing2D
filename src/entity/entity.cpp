#include "entity.h"
#include "game\map.h"
#include "game\gameCommands.h"
#include "lua\luaContext.h"
#include "entity\entityState.h"
#include "entity\camera.h"
#include "movements\movement.h"

Entity::Entity():
	mName(""),
	mBounding(),
	mOrigin(0, 0),
	mLayer(0),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mIsInitialized(false),
	mIsDrawOnYOrder(false),
	mDebugSprite(nullptr)
{
}

Entity::Entity(const string & name, const Point2 & pos, const Size & size, int layer):
	mName(name),
	mBounding(pos, size),
	mOrigin(),
	mLayer(layer),
	mType(EntityType::UNKNOW),
	mState(nullptr),
	mMovement(nullptr),
	mIsInitialized(false),
	mIsDrawOnYOrder(false),
	mDebugSprite(nullptr)
{
}

/**
*	\brief entity刷新动作
*
*	如果存在状态组件则更新状态组件
*/
void Entity::Update()
{
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
	for (auto& nameSprite : mSprites)
	{
		auto sprite = nameSprite.sprite;
		if (sprite != nullptr)
		{
			GetMap().DrawOnMap(*sprite);
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

	mIsInitialized = true;
}

void Entity::ClearMovements()
{
}

const string Entity::GetLuaObjectName() const
{
	return string();
}

void Entity::DrawDebugBounding()
{
	if (mDebugSprite == nullptr)
	{
		mDebugSprite = std::make_shared<Sprite>(Color4B(rand() % (255), rand() % (255), rand() % (255), 255), Size(0, 0));
	}

	mDebugSprite->SetPos(mBounding.GetPos());
	mDebugSprite->SetSize(mBounding.GetSize());
	GetMap().DrawOnMap(*mDebugSprite);
}

void Entity::NotifyCommandPressed(const GameCommand & command)
{
	// do nothing
}

void Entity::NotifyCommandReleased(const GameCommand & command)
{
	// do nothing
}

void Entity::NotifyMovementChanged()
{
	if (mState != nullptr)
	{
		mState->NotifyMovementChanged();
	}
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

const Map & Entity::GetMap() const
{
	return *mMap;
}

Game & Entity::GetGame()
{
	Debug::CheckAssertion(mMap != nullptr,
		"The invalid entity withou map.");
	return mMap->GetGame();
}

const std::shared_ptr<EntityState>& Entity::GetState()
{
	return mState;
}

/**
*	\brief 设置当前状态
*/
void Entity::SetState(const std::shared_ptr<EntityState>& state)
{
	auto oldState = GetState();
	if (state != oldState)
	{
		if (oldState != nullptr)
		{
			oldState->Stop(*state);
		}
		mState = state;
		state->Start(*oldState);
	}
}

void Entity::UpdateState()
{
	if (mState != nullptr)
	{
		mState->Update();
	}
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
void Entity::SetSize(const Size & size)
{
	mBounding.SetSize(size.width, size.height);
}
Size Entity::GetSize() const
{
	return mBounding.GetSize();
}
const string& Entity::GetName()const
{
	return mName;
}
EntityType Entity::GetEntityType()const
{
	return mType;
}