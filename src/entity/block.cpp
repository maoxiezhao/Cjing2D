#include "block.h"
#include "lua\luaContext.h"
#include "entity\player.h"
#include "movements\attachMovement.h"
#include "core\system.h"

Block::Block(const std::string & name, int layer, const Point2 & pos,
	const std::string& spriteName, bool canPushed, bool canPulled, bool canDesotry):
	Entity(name, "Block", pos, Size(32, 32), layer),
	mCanPushed(canPushed),
	mCanPulled(canPulled),
	mCanDestory(canDesotry),
	mCanMoveDate(0),
	mMovingDelay(500),
	mMovingEntity(nullptr)
{
	if (!spriteName.empty())
		CreateAnimationSprite(animateSetID, spriteName);

	SetSize({32, 20});
	SetNotifySize({36, 24});
	SetOrigin({ 0, -20 });
	SetDrawOnYOrder(true);
	SetCollisionMode(CollisionMode::COLLISION_FACING);
}

bool Block::IsObstacle(Entity & entity) const
{
	return entity.IsObstacleBlock();
}

bool Block::StartMoveByPushed(Entity & entity)
{
	// enable judge
	uint32_t now = System::Now();
	if (GetMovement() != nullptr ||
		now < mCanMoveDate || !mCanPushed ||
		entity.GetEntityType() != EntityType::PLAYRE)
		return false;

	// set movement
	Point2 diff = GetPos() - entity.GetPos();
	auto movement = std::make_shared<AttachMovement>(&entity, diff, false);
	StartMovement(movement);

	// record moving entity
	mMovingEntity = std::static_pointer_cast<Entity>(entity.shared_from_this());

	return true;
}

bool Block::StopMoveByPushed()
{
	mCanMoveDate = System::Now() + mMovingDelay;
	ClearMovements();
	mMovingEntity = nullptr;

	return true;
}

void Block::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	otherEntity.NotifyCollisionWithBlock(*this);
}

/**
*	\brief 当该block被设置为facingEntity且按下交互键时触发,此时player进入grabbing模式
*/
bool Block::NotifyCommandInteractPressed(Entity & interactEntity)
{
	if (interactEntity.GetFacingEntity() == this &&
		interactEntity.GetEntityType() == EntityType::PLAYRE)
	{
		auto& player = dynamic_cast<Player&>(interactEntity);
		player.Grabbing();
		return true;
	}
	return false;
}

void Block::NotifyObstacleReached()
{
	if (mMovingEntity != nullptr)
	{
		auto player = std::static_pointer_cast<Player>(mMovingEntity);
		player->StopGrabbing();
	}

	Entity::NotifyObstacleReached();
}

bool Block::IsCanPushed() const
{
	return mCanPushed;
}

void Block::SetCanPushed(bool pushed)
{
	mCanPushed = pushed;
}

bool Block::IsCanPulled() const
{
	return mCanPulled;
}

void Block::SetCanPulled(bool pulled)
{
	mCanPulled = pulled;
}

bool Block::IsCanDestroy() const
{
	return mCanDestory;
}

void Block::SetCanDestroy(bool destroy)
{
	mCanDestory = destroy;
}

EntityType Block::GetEntityType() const
{
	return EntityType::BLOCK;
}

const string Block::GetLuaObjectName() const
{
	return LuaContext::module_block_name;
}
