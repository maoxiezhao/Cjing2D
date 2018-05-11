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
			continue;
	
		namedSprite.sprite->Update();
	}
	ClearRemovedSprite();

	// movement
	if (mMovement != nullptr)
	{
		mMovement->Update();

		// ����ƶ���ϣ����Ƴ�movement
		if (mMovement != nullptr && mMovement->IsFinished())
			StopMovement();
	}

	// state
	UpdateState();
}

/**
*	\brief ����entity��map
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
*	\brief ��ɳ�ʼ������notifyMapStartʱ����
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
*	\brief ����״̬Ϊ����ע״̬
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
*	\brief ��Ӧλ�ñ䶯
*/
void Entity::NotifyPositonChanged()
{
	NotifyBoundingRectChange();

	// �����������ײʱ����Ҫ��֪����entity�Ը�entity
	if (IsHaveCollision())
		CheckCollisionFromEntities();

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
*	\brief ��Ӧ������
*/
void Entity::NotifyBeRemoved()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnRemoved");
	mBeRemoved = true;
}

/**
*	\brief ��ӦfacingEntity�ĸı�
*/
void Entity::NotifyFacingEntityChanged(Entity * entity)
{
	// do nothing
}

/**
*	\brief ��ӦoverlapEntity�ĸı�
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
*	\brief ��entity�İ�Χ����ı�ʱ����Ҫentities�ı��Ĳ����ķֲ�
*/
void Entity::NotifyBoundingRectChange()
{
	if (IsOnMap())
		GetMap().GetEntities().NotifyEntityRectChanged(*this);
}

/**
*	\brief ��entity�����ϰ���ʱ��Ӧ
*/
void Entity::NotifyObstacleReached()
{
	GetLuaContext()->CallFunctionWithUserdata(*this, "OnNotifyObstacleReached");
}

/**
*	\brief ��entity����Enemyʱ��Ӧ
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
*	\brief ��entityg����Playerʱ��Ӧ
*/
void Entity::NotifyAttackPlayer(Player & player, EntityAttack attack, EntityReactionType reaction)
{
}

/**
*	\brief ��Ӧ�������İ���
*	\param interactEntity ���𽻻���ʵ��
*	\return true�������ɹ�������false��ʵ���޷�����
*
*	���ڵ��ߡ�NPC���ڸú�����������Ӧ	
*/
bool Entity::NotifyCommandInteractPressed(Entity& interactEntity)
{
	return false;
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
	SetLuaContext(&map->GetLuaContext());

	if (!mIsInitialized && mMap->IsLoaded())
	{	// ��������Ϸ��ʵʱ����entity
		// ���ڼ����ͼʱ��ʼ��
		Initalized();
	}
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
	NotifyBoundingRectChange();

	return sprite;
}

/**
*	\brief ����animaiton,������sprite
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
*	\brief ɾ������sprite
*
*	������������������е�sprite update֮�����
*	clearRemovedSpritesִ��
*/
void Entity::ClearSprites()
{
	for (auto& sprite : mSprites)
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
			// ������oldState::Stop�зǷ��ı��˵�ǰ��state
			Debug::Error("Invalid Change Entity State:" + oldState->GetName());
			return;
		}
	}
	// ������һ���ӳ�ɾ����һ֡���ܻᱻ���õ�
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
*	\brief ����ʵ��������ʵ��֮���Ƿ�����ײ
*/
void Entity::CheckCollisionWithEntities()
{
	if (!IsOnMap())
		return;

	// ��ײ���
	GetMap().CheckCollisionWithEntities(*this);

	// ���ؼ���ײ���
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

	// ���ؼ���ײ���
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
*	\brief ��⾫������ؼ���ײ���
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
*	\brief ��collisonMode������collision_noneʱ������ײ
*/
bool Entity::IsHaveCollision() const
{
	return mCollisionMode != COLLISION_NONE;
}

/**
*	\brief ����ʵ��������ʵ��������ײ
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
*	\brief ����entity��ע�ⷽ��
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
*	\brief ����entityע��ķ���
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
*	\brief �Ƿ�����ƶ���Ŀǰ��player���Զ�����entity�����ƶ�
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

