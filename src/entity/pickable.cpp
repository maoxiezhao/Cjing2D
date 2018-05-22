#include"entity\pickable.h"
#include"game\map.h"
#include"game\item.h"
#include"movements\targetMovement.h"
#include"movements\sequenceMovement.h"
#include"lua\luaContext.h"

namespace
{
	const Size DefaultItemSize = { 32, 32 };	// Ĭ�ϵ�item��С
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
*	\brief ������Ʒ״̬
*
*	�����Ʒ����ͣ��ʱ�䣬�򵱳���ʱ��󣬴Ӹõ�ͼ��
*	�Զ����ٸ���Ʒ
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
*	\brief ������Ʒ
*/
void Pickable::Draw()
{
	Entity::Draw();

	GetMap().DrawOnMap(*mItemSprite); 
}

/**
*	\brief ���ص�ǰentity����
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
*	\brief ��Ӧ��ײ����
*	\param otherEntity ������ײ������ʵ��
*	\param collisonMode ��������ײ����
*/
void Pickable::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	if (otherEntity.GetEntityType() == EntityType::PLAYRE)
	{
		// ����Զ���ȡ����ֱ�ӳ��Ը�player
		auto& item = mItemAcquired.GetItem();
		if (item.IsAutoPicked())
		{
			TryGiveItemToPicker(otherEntity);
		}
	}
}

/**
*	\brief �����߲����Զ�ʰȡʱ������ʱʰȡ�õ���
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
*	\brief ����Entity��focus���ı�ʱ����
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
*	\brief ������Ʒ�ľ���ͼƬ
*/
bool Pickable::InitSprites()
{
	// ������ƷͼƬ
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
	// �����������ѵ���Ӱ�͸���Ч��
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
*	\brief ��ͼ����Ʒ�ַ���ʰȡ��
*
*	����Ϊ�п���ʧ�ܣ���Ϊ���������ܣ�ʰȡ���޷�
*	��ȡ����Ʒ��
*/
void Pickable::TryGiveItemToPicker(Entity& picker)
{
	Item& item = mItemAcquired.GetItem();

	// ���picker��ȡ�ɹ�����ɾ��,�����ܴ��ڻ�ȡʧ��
	// getHero().canPickItem(item)
	if (!mCanPicked )
	{
		return;
	}

	// ��ȡitem
	bool pickSucceesed = mItemAcquired.GiveItemToPlayer(picker);
	if (pickSucceesed)
	{
		// ���ʰȡ��Ч���ڣ��򲥷�
		const std::string pickedSound = item.GetPickedSound();
		if (!pickedSound.empty())
		{
			// sound::play(pickedSound)
		}

		// �Ƴ�pickable
		RemoveFromMap();
	}	
}
