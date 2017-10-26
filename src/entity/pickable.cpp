#include"entity\pickable.h"
#include"game\map.h"
#include"game\item.h"
#include"movements\targetMovement.h"
#include"movements\sequenceMovement.h"

namespace
{
	const Size DefaultItemSize = { 32, 32 };	// Ĭ�ϵ�item��С
}

Pickable::Pickable(const std::string itemName, int layer, const Point2 & pos, ItemAcquired& itemAcquired):
	Entity(itemName, pos, DefaultItemSize, layer ),
	mItemAcquired(itemAcquired),
	mItemSprite(nullptr),
	mShadowSprite(nullptr),
	mMovemment(nullptr),
	mCanPicked(true)
{
	SetCollisionMode(CollisionMode::COLLISION_OVERLAPING);

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
	return string();
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
		TryGiveItemToPicker(otherEntity);
	}
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
	mItemSprite->SetPos(GetPos());

	// �����������ѵ���Ӱ�͸���Ч��
	// PS.�Ƿ���Ҫ��Ӱ�͸���Ч���ɵ��߱�����
	// Ŀǰ���ڲ��ԣ����ж�
	mShadowSprite = Entity::CreateAnimationSprite("entities/shadow", "big");
	mShadowSprite->SetAnchorFloat(-0.3f, -1.2f);

	// movement
	mMovemment = std::make_shared<SequenceMovement>(true, true);
	Point2 upPoint = GetPos() + Point2(0, -4);
	auto upMovement = std::make_shared<TargetMovement>(nullptr, upPoint, 10, true);
	mMovemment->AddMovement(upMovement);
	Point2 downPoint = GetPos();
	auto downMovement = std::make_shared<TargetMovement>(nullptr, downPoint, 10, true);
	mMovemment->AddMovement(downMovement);
	mItemSprite->StartMovement(mMovemment);

	// status
	SetSize({ 16, 16 });
	SetOrigin({ 8, 13 });

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

	// ���ʰȡ��Ч���ڣ��򲥷�
	const std::string pickedSound = item.GetPickedSound();
	if (!pickedSound.empty())
	{
		// sound::play(pickedSound)
	}

	// �Ƴ�pickable
	RemoveFromMap();

	// ��ȡitem
	mItemAcquired.GiveItemToPlayer();
}
