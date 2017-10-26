#include "entity.h"
#include "game\map.h"
#include "game\gameCommands.h"
#include "lua\luaContext.h"
#include "entity\entities.h"
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
	mVisibled(true),
	mDebugSprite(nullptr),
	mEnabled(true),
	mCollisionMode(COLLISION_NONE),
	mBeRemoved(false)
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
	mVisibled(true),
	mDebugSprite(nullptr),
	mEnabled(true),
	mCollisionMode(COLLISION_NONE),
	mBeRemoved(false)
{
}

Entity::~Entity()
{
	ClearSprites();
	ClearRemovedSprite();
	ClearMovements();
}

/**
*	\brief entityˢ�¶���
*
*	�������״̬��������״̬���
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
		// ����ƶ���ϣ����Ƴ�movement
		if (mMovement != nullptr && mMovement->IsFinished())
		{
			StopMovement();
		}
	}

	// state
	UpdateState();
}

/**
*	\brief ����entity��map
*/
void Entity::Draw()
{
	for (auto& nameSprite : mSprites)
	{
		auto sprite = nameSprite.sprite;
		if (sprite != nullptr)
		{
			GetMap().DrawOnMap(*sprite, GetPos());
		}
	}
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
*	\brief ��Ӧλ�ñ䶯
*/
void Entity::NotifyPositonChanged()
{
	CheckCollisionWithEntities();
}

/**
*	\brief ��Ӧ��ײ����
*	\param otherEntity ������ײ������ʵ��
*	\param collisonMode ��������ײ����
*/
void Entity::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
}

/**
*	\brief ��Ӧ������
*/
void Entity::NotifyBeRemoved()
{
	// luaContext->onRemovedEneity(*this) ��Ӧlua
	mBeRemoved = true;
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

/**
*	\brief �Ƿ��ڵ�ͼ��
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
*	\brief ����enitty�����õĵ�ͼ������
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
*	\brief ����sprite,������sprite
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
*	\brief ����animaiton,������sprite
*/
AnimationSpritePtr Entity::CreateAnimationSprite(const string & animationSetId, const string & animationID)
{
	// animationSprite
	AnimationSpritePtr animationSprite = std::make_shared<AnimationSprite>(animationSetId);
	animationSprite->SetCurrAnimation(animationID);
	animationSprite->SetPos(GetPos());

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

/**
*	\brief ɾ������sprite
*
*	������������������е�sprite update֮�����
*	clearRemovedSpritesִ��
*/
void Entity::ClearSprites()
{
	for (auto sprite : mSprites)
	{
		sprite.removed = true;
	}
}

/**
*	\brief �ͷ����б����Ϊremoved��sprite
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

/**
*	\brief ����ʵ��������ʵ��֮���Ƿ�����ײ
*/
void Entity::CheckCollisionWithEntities()
{
	if (!IsOnMap())
	{
		return;
	}
	GetMap().CheckCollisionWithEntities(*this);
}

/**
*	\brief ���ʵ������ײ���
*	\param otherEntity ����������ʵ��
*
*	��������ײ���󣬸�����ײ���͵���notifyCollision,
*	��Ӧ�����¼�
*/
void Entity::CheckCollision(Entity & otherEntity)
{
	if (!IsHaveCollision() || this == &otherEntity)
	{
		return;
	}
	// �㼶��Ҫ��ͬ,���趨Ϊͬ�㼶��ײ
	if (GetLayer() != otherEntity.GetLayer())
	{
		return;
	}
	// ���ݵ�ǰ����ײģʽ������ײ
	if (HasCollisionMode(CollisionMode::COLLISION_OVERLAPING) &&
		TestCollisionWithRect(otherEntity))
	{
		NotifyCollision(otherEntity, CollisionMode::COLLISION_OVERLAPING);
	}
	else if (HasCollisionMode(CollisionMode::COLLISION_CONTAINING) &&
		TestCollisionContaining(otherEntity))
	{
		NotifyCollision(otherEntity, CollisionMode::COLLISION_CONTAINING);
	}
}

/**
*	\brief ��collisonMode������collision_noneʱ������ײ
*/
bool Entity::IsHaveCollision() const
{
	return mCollisionMode != COLLISION_NONE;
}

/**
*	\brief ����ʵ��������ʵ��������ײ
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
*	\brief ���õ�ǰ��ײģʽ
*/
void Entity::SetCollisionMode(CollisionMode collisionMode)
{
	mCollisionMode |= collisionMode;
}

/**
*	\brief ���ص�ǰ�Ƿ���ڸ���ײģʽ
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
*	\brief ������Ļ����
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

