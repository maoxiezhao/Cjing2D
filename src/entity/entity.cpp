#include "entity.h"
#include "game\map.h"
#include "game\gameCommands.h"
#include "lua\luaContext.h"
#include "entity\entityState.h"
#include "movements\movement.h"

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
*	\brief entityˢ�¶���
*
*	�������״̬��������״̬���
*/
void Entity::Update()
{
	// movement
	if (mMovement != nullptr)
	{
		mMovement->Update();
		// ����ƶ���ϣ����Ƴ�movement
		if (mMovement != nullptr && mMovement->IsFinished())
		{
			StopMovement();
		}
	}

	// state
	UpdateState();
}

void Entity::Draw()
{
}

/**
*	\brief ��ɳ�ʼ������notifyMapStartʱ����
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

void Entity::NotifyMovementChanged()
{
	if (mState != nullptr)
	{
		mState->NotifyMovementChanged();
	}
}

/**
*	\brief ����ͼ��ɼ��غ�ִ����ɳ�ʼ��
*/
void Entity::NotifyMapStarted()
{
	if (!mIsInitialized)
	{
		Initalized();
	}
}

/**
*	\biref ���õ�ǰ��ͼ
*
*	���е�entity��setmap֮��������ȫ����ʼ��
*/
void Entity::SetMap(Map * map)
{
	mMap = map;
}

Map & Entity::GetMap()
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
*	\brief ���õ�ǰ״̬
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