#include"entity\pickable.h"
#include"game\map.h"
#include"game\item.h"
#include"movements\targetMovement.h"
#include"movements\sequenceMovement.h"
#include"lua\luaContext.h"

namespace
{
	const Size DefaultItemSize = { 32, 32 };	// 默认的item大小
}

Pickable::Pickable(const std::string itemName, int layer, const Point2 & pos, ItemAcquired& itemAcquired):
	Entity(itemName, itemName, pos, DefaultItemSize, layer ),
	mItemAcquired(itemAcquired),
	mItemSprite(nullptr),
	mShadowSprite(nullptr),
	mMovemment(nullptr),
	mCanPicked(true)
{
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);
	SetDrawOnYOrder(true);

	if (!InitSprites())
	{
		Debug::Warning("Failed to init item sprites names:" + itemName);
	}
}

/**
*	\brief 更新物品状态
*
*	如果物品存在停留时间，则当超过时间后，从该地图中
*	自动销毁该物品
*/
void Pickable::Update()
{
	auto& item = mItemAcquired.GetItem();
	if (!item.IsAutoPicked() && IsFosused())
	{
		mItemSprite->SetOutLine(0.5f);
	}
	else
	{
		mItemSprite->SetOutLine(0.0f);
	}

	mItemSprite->Update();
	Entity::Update();
}

/**
*	\brief 绘制物品
*/
void Pickable::Draw()
{
	Entity::Draw();

	GetMap().DrawOnMap(*mItemSprite); 
}

/**
*	\brief 返回当前entity类型
*/
EntityType Pickable::GetEntityType() const
{
	return EntityType::PICKABLE;
}

const string Pickable::GetLuaObjectName() const
{
	return LuaContext::module_entity_name;
}

Point2 Pickable::GetBillBoardPos() const
{
	Point2 positivePos = GetPositivePos();
	Size entitySize = GetSize();
	positivePos.y = positivePos.y - entitySize.height - 5;
	return positivePos;
}

/**
*	\brief 响应碰撞函数
*	\param otherEntity 发生碰撞的其他实体
*	\param collisonMode 发生的碰撞类型
*/
void Pickable::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	if (otherEntity.GetEntityType() == EntityType::PLAYRE)
	{
		// 如果自动获取，则直接尝试给player
		auto& item = mItemAcquired.GetItem();
		if (item.IsAutoPicked())
		{
			TryGiveItemToPicker(otherEntity);
		}
	}
}

/**
*	\brief 当道具不是自动拾取时，互动时拾取该道具
*/
bool Pickable::NotifyCommandInteractPressed(Entity& interactEntity)
{
	auto& item = mItemAcquired.GetItem();
	if (!item.IsAutoPicked())
	{
		TryGiveItemToPicker(interactEntity);
	}
	return false;
}

/**
*	\brief 当该Entity的focus被改变时触发
*/
void Pickable::NotifyFocusChange(bool focused)
{
	auto& item = mItemAcquired.GetItem();
	if (focused)
		GetLuaContext()->CallFunctionWithUserdata(item, "OnGainFocus", 
			[&](lua_State*l)->int {
			GetLuaContext()->PushUserdata(l, *this);
			return 1;
		});
	else
		GetLuaContext()->CallFunctionWithUserdata(item, "OnLoseFocus", 
			[&](lua_State*l)->int {
			GetLuaContext()->PushUserdata(l, *this);
			return 1;
		});
}

/**
*	\brief 创建商品的精灵图片
*/
bool Pickable::InitSprites()
{
	// 创建物品图片
	const std::string itemName = mItemAcquired.GetItemName();
	const std::string itemsSet = "entities/items";
	mItemSprite = std::make_shared<AnimationSprite>(itemsSet);
	if (!mItemSprite->HasAnimation(itemName))
	{
		Debug::Warning("Failed to create pickable sprite, the itemsSet:" + itemsSet + 
			" has no item animation:" + itemName);
		return false;
	}
	mItemSprite->SetCurrAnimation(itemName);
	mItemSprite->StartAnimation();

	auto spriteSize = mItemSprite->GetSize();
	SetSize(spriteSize);

	Point2 spritePos = { GetPos().x, GetPos().y - spriteSize.height};
	mItemSprite->SetPos(spritePos);

	auto& item = mItemAcquired.GetItem();
	// 创建用于提醒的阴影和浮动效果
	if (item.IsHasShadow())
	{
		mShadowSprite = Entity::CreateAnimationSprite("entities/shadow", "big");
		mShadowSprite->SetPos({ spriteSize.width / 3, 0 });
		mShadowSprite->SetAnchorFloat(0.0f, 0.0f);
	}
	// movement
	if (item.IsHasFlowEffect())
	{
		mMovemment = std::make_shared<SequenceMovement>(true, true);
		Point2 upPoint = spritePos + Point2(0, -4);
		auto upMovement = std::make_shared<TargetMovement>(nullptr, upPoint, 10, true);
		mMovemment->AddMovement(upMovement);
		Point2 downPoint = spritePos;
		auto downMovement = std::make_shared<TargetMovement>(nullptr, downPoint, 10, true);
		mMovemment->AddMovement(downMovement);
		mItemSprite->StartMovement(mMovemment);
	}

	// status
	//SetSize({ 16, 16 });
	//SetOrigin({ spriteSize.width/ 2 , 0});

	return true;
}

/**
*	\brief 试图将物品分发给拾取者
*
*	该行为有可能失败，因为存在许多可能，拾取者无法
*	获取该物品。
*/
void Pickable::TryGiveItemToPicker(Entity& picker)
{
	Item& item = mItemAcquired.GetItem();

	// 如果picker获取成功，则删除,但可能存在获取失败
	// getHero().canPickItem(item)
	if (!mCanPicked )
	{
		return;
	}

	// 获取item
	bool pickSucceesed = mItemAcquired.GiveItemToPlayer(picker);
	if (pickSucceesed)
	{
		// 如果拾取音效存在，则播放
		const std::string pickedSound = item.GetPickedSound();
		if (!pickedSound.empty())
		{
			// sound::play(pickedSound)
		}

		// 移除pickable
		RemoveFromMap();
	}	
}
