#include "chest.h"
#include "lua\luaContext.h"
#include "game\animationSprite.h"
/**
*	\brief Chset��˼·����������߼�������templNameģ�壬templName��Lua�ļ�
*	�ᱻ���أ�Ȼ���Ƿ�򿪣��򿪺���߼�����templģ�����������������ѡ��Ŀ��ؽű���
*/

Chest::Chest(const std::string & name, const std::string & templName, int layer,
	const Point2 & pos, const std::string & spriteName):
	Entity(name, templName, pos, Size(32, 32), layer),
	mIsOpen(false),
	mSpriteName(spriteName)
{
	if (!spriteName.empty())
	{
		std::string statusString = IsOpen() ? "_open" : "_close";
		CreateAnimationSprite(animateSetID, spriteName + statusString);
	}

	SetSize({ 32, 20 });
	SetNotifySize({ 36, 24 });
	SetOrigin({ 0, -20 });
	SetDrawOnYOrder(true);
	SetCollisionMode(CollisionMode::COLLISION_FACING);
}

bool Chest::CanOpen()
{
	return GetLuaContext()->CallFunctionWithUserdata(*this, "OnCanOpen");
}

bool Chest::IsOpen() const
{
	return mIsOpen;
}

void Chest::SetItems(const std::string & items)
{
	mItems = items;
}

std::string Chest::GetItmes() const
{
	return mItems;
}

bool Chest::IsObstacle(Entity & entity) const
{
	return entity.isObstacleChest();
}

void Chest::NotifyCollision(Entity & otherEntity, CollisionMode collisionMode)
{
	otherEntity.NotifyCollisionWithChest(*this);
}

/**
*	\brief �����ű��䰴�½�����ʱ�����Դ򿪱���
*/
bool Chest::NotifyCommandInteractPressed(Entity & interactEntity)
{
	if (interactEntity.GetFacingEntity() == this &&
		interactEntity.GetEntityType() == EntityType::PLAYRE &&
		!IsOpen())
	{
		if (CanOpen())
		{
			SetOpen(true);

			GetLuaContext()->CallFunctionWithUserdata(*this, "OnOpenSucceed",
				[&](lua_State*l)->int {
				GetLuaContext()->PushUserdata(l, interactEntity);
				return 1;
			});
		}
		else
		{
			GetLuaContext()->CallFunctionWithUserdata(*this, "OnOpenFailed",
				[&](lua_State*l)->int {
				GetLuaContext()->PushUserdata(l, interactEntity);
				return 1;
			});
		}
		return true;
	}

	return false;
}

void Chest::NotifyBeforeCreated()
{
	std::string path = "entities/common/" + GetTemplName() + ".lua";
	GetLuaContext()->CallFileWithUserdata(path, *this);
}

void Chest::NotifyAfterCreated()
{
}

EntityType Chest::GetEntityType() const
{
	return EntityType::CHEST;
}

const string Chest::GetLuaObjectName() const
{
	return LuaContext::module_chest_name;
}

/**
*	\brief ���ñ���Ŀ���״̬
*/
void Chest::SetOpen(bool opened)
{
	if (opened != mIsOpen)
	{
		mIsOpen = opened;

		auto chestSprite = GetFirstSprite();
		if (chestSprite && chestSprite->IsAnimationed())
		{
			auto sprite = std::static_pointer_cast<AnimationSprite>(chestSprite);
			std::string statusString = IsOpen() ? "_open" : "_close";
			sprite->SetCurrAnimation( mSpriteName + statusString);
		}

	}
}

