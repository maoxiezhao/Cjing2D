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
			GetMap().DrawOnMap(*sprite);
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