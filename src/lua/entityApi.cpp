#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"core\fileData.h"
#include"game\map.h"
#include"game\mapData.h"
#include"game\itemAcquired.h"

#include"entity\tile.h"
#include"entity\entities.h"
#include"entity\tileset.h"
#include"entity\pickable.h"
#include"entity\enemy.h"
#include"entity\player.h"
#include"entity\bullet.h"
#include"entity\destination.h"
#include"entity\weaponInstance.h"
#include"entity\block.h"

const std::string LuaContext::module_entity_name = "Entity";
const std::string LuaContext::module_player_name = "Player";
const std::string LuaContext::module_enemy_name = "Enemy";
const std::string LuaContext::module_entity_bullet_name = "Bullet";
const std::string LuaContext::module_weapon_instance = "WeaponInstance";
const std::string LuaContext::module_block_name = "Block";

const std::vector<std::string> mEntityClassNames = {
	LuaContext::module_entity_name,
	LuaContext::module_enemy_name,
	LuaContext::module_player_name,
	LuaContext::module_entity_bullet_name,
	LuaContext::module_weapon_instance,
	LuaContext::module_block_name,
};

void LuaContext::RegisterEntityModule()
{
	// entity base
	LuaBindClass<Entity> entityClass(l, "Entity");
	entityClass.AddDefaultMetaFunction();
	entityClass.AddFunction("SetDebugDraw", &Entity::SetDebugDrawBounding);
	entityClass.AddMethod("CreateSprite", entity_api_create_sprite);
	entityClass.AddMethod("ClearSprites", &Entity::ClearSprites);
	entityClass.AddMethod("GetName",  &Entity::GetName);
	entityClass.AddMethod("SetName",  &Entity::SetName);
	entityClass.AddMethod("SetSize",  &Entity::SetSize);
	entityClass.AddMethod("GetSize",  &Entity::GetSize);
	entityClass.AddMethod("SetOrigin",&Entity::SetOrigin);
	entityClass.AddMethod("GetPos",   &Entity::GetPos);
	entityClass.AddMethod("GetCenterPos", &Entity::GetCenterPos);
	entityClass.AddMethod("SetPos",  &Entity::SetPos);
	entityClass.AddMethod("GetBillBoardPos", &Entity::GetBillBoardPos);
	entityClass.AddMethod("GetLayer", &Entity::GetLayer);
	entityClass.AddMethod("GetAttachPos", &Entity::GetAttachPos);
	entityClass.AddMethod("SetFacingDegree", &Entity::SetFacingDegree);
	entityClass.AddMethod("GetFacingDegree", &Entity::GetFacingDegree);
	entityClass.AddMethod("StopMovement", &Entity::StopMovement);
	entityClass.AddMethod("SetEnable", &Entity::SetEnable);
	entityClass.AddMethod("GetGame",   entity_api_get_game);
	entityClass.AddMethod("GetType",   entity_api_get_type);
	entityClass.AddMethod("GetMap",    entity_api_get_map);
	entityClass.AddMethod("GetSprite", entity_api_get_sprite);
	entityClass.AddMethod("GetUTable", userdata_get_utable);

	// player 
	LuaBindClass<Player> playerClass(l, module_player_name, module_entity_name);
	playerClass.AddDefaultMetaFunction();

	// Enemy
	LuaBindClass<Enemy> enemyClass(l, "Enemy", "Entity");
	enemyClass.AddDefaultMetaFunction();
	enemyClass.AddMethod("Killed", &Enemy::Killed);
	enemyClass.AddMethod("SetDeadAnimTime", &Enemy::SetDeadAnimTime);
	enemyClass.AddMethod("TryHurt", entity_api_try_hurt);		// try hurt方法应该将要实现为AttackEntity的基类方法
	enemyClass.AddMethod("IsHurting", &Enemy::IsHurting);

	// Bullet
	LuaBindClass<Bullet> bulletClass(l, "Bullet", "Entity");
	bulletClass.AddDefaultMetaFunction();
	bulletClass.AddMethod("SetDisappearAnimTime", &Bullet::SetDisapearAnimTime);

	// Weapon instance
	LuaBindClass<WeaponInstance> weaponClass(l, module_weapon_instance, module_entity_name);
	weaponClass.AddDefaultMetaFunction();

	// Block 
	LuaBindClass<Block> blockClass(l, module_block_name, module_entity_name);
	blockClass.AddDefaultMetaFunction();
}

/**
*	\brief 所有的entity的lua构造函数
*/
std::map<EntityType, lua_CFunction> LuaContext::mEntitityCreaters = 
{
	{ EntityType::TITLE, entity_api_create_title},
	{ EntityType::DESTIMATION, entity_api_create_destimation },
	{ EntityType::DYNAMIC_TITLE, entity_api_create_dynamic_title },
	{ EntityType::PICKABLE, entity_api_create_pickable },
	{ EntityType::ENEMEY, entity_api_create_enemy },
	{ EntityType::BULLET, entity_api_create_bullet },
	{ EntityType::BLOCK, entity_api_create_block },
};

/**
*	\brief entity_title的创建函数
*/
int LuaContext::entity_api_create_title(lua_State* l) 
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));

		TileInfo tileInfo;
		tileInfo.mLayer = entityData.GetLayer();
		tileInfo.mPos = entityData.GetPos();
		tileInfo.mSize.width = entityData.GetValueInteger("width");
		tileInfo.mSize.height = entityData.GetValueInteger("height");
		tileInfo.mPatternID = entityData.GetValueInteger("pattern");

		const Tileset& tileset = map.GetTileset();
		auto pattern = &tileset.GetTilePattern(tileInfo.mPatternID);
		tileInfo.mPattern = pattern;
		tileInfo.mGround = pattern->GetGround();
		map.GetEntities().AddTile(tileInfo);

		lua_pushboolean(l, true);
		return 1;
	});
}

int LuaContext::entity_api_create_destimation(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));

		auto destination = std::make_shared<Destination>(
			entityData.GetName(),
			entityData.GetPos(),
			entityData.GetLayer()
			);
		map.GetEntities().AddEntity(destination);
		PushUserdata(l, *destination);
		return 1;
	});
}

int LuaContext::entity_api_create_dynamic_title(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));

		lua_pushboolean(l, true);
		return 1;
	});
}

/**
*	\brief entity_title的创建函数
*/
int LuaContext::entity_api_create_pickable(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		//GetLuaContext(l).PrintLuaStack(l);
		Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));
	
		auto pickable = std::make_shared<Pickable>(
			entityData.GetValueString("itemName"),
			entityData.GetLayer(), 
			entityData.GetPos(),
			ItemAcquired(map.GetGame(), entityData.GetValueString("itemName"), entityData.GetValueInteger("itemCount"))
		);

		map.GetEntities().AddEntity(pickable);
		lua_pushboolean(l, true);
		return 1;
	});
}


/**
*	\brief entity_enemy的创建函数
*/
int LuaContext::entity_api_create_enemy(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));
		Game& game = map.GetGame();

		auto enemy =  Enemy::Create(game,
			entityData.GetName(), 
			entityData.GetValueString("templ"),
			entityData.GetLayer(), 
			entityData.GetPos());

		enemy->SetDirection(static_cast<Direction4>(
			entityData.GetValueInteger("direction")));
		
		if (map.IsStarted())
		{
			if (lua_isfunction(l, 3))
			{			
				PushUserdata(l, *enemy);
					// func enemy
				LuaTools::CallFunction(l, 1, 0, "enemy init");

				lua_pushnil(l);	// align stack??
			}
			map.GetEntities().AddEntity(enemy);
						// --
			PushUserdata(l, *enemy);
						// enemy
			return 1;
		}
		return 0;
	});
}

/**
*	\brief entity_bullet的创建函数
*/
int LuaContext::entity_api_create_bullet(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));

		auto bullet = Bullet::Create(
			entityData.GetValueString("templ"), 
			entityData.GetLayer());

		if (map.IsStarted())
		{
			if (lua_isfunction(l, 3))
			{
				PushUserdata(l, *bullet);
				LuaTools::CallFunction(l, 1, 0, "entityInit");
				lua_pushnil(l);	
			}
			map.GetEntities().AddEntity(bullet);
			PushUserdata(l, *bullet);
			return 1;
		}
		return 0;
	});
}



/**
*	\brief cr的创建函数
*/
int LuaContext::entity_api_create_block(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));

		auto block = std::make_shared<Block>(
			entityData.GetName(),
			entityData.GetLayer(),
			entityData.GetPos(),
			entityData.GetValueString("sprite"),
			entityData.GetValueBoolean("push"),
			entityData.GetValueBoolean("pull"),
			entityData.GetValueBoolean("destory"));

		map.GetEntities().AddEntity(block);
		PushUserdata(l, *block);
		return 1;
	});
}

/**
*	\brief 创建对应的entity
*
*	其中map和该userdata会作为参数传入, initCallback会在实体创建完后
*	地图创建之前调用，一般可用于特殊的初始化需要
*/
bool LuaContext::CreateEntity(const EntityData& entityData, Map& map, LuaRef& initCallback, bool clearStack)
{
	const EntityType type = entityData.GetEntityType();
	auto entityCreater = mEntitityCreaters.find(type);
	Debug::CheckAssertion(entityCreater != mEntitityCreaters.end(),
		"Invalid entity type in entity creating.");

	// do creating func,参数依次为
	lua_CFunction function = entityCreater->second;
	lua_pushcfunction(l, function);
	PushMap(l, map);
	lua_pushlightuserdata(l, const_cast<EntityData*>(&entityData));

	bool hasInitCallback = !initCallback.IsEmpty();
	int argNum = hasInitCallback ? 3 : 2;
	if (hasInitCallback){
		initCallback.Push();
	}

	bool result =  LuaTools::CallFunction(l, argNum, 1, "create entity");
	if (clearStack)
		lua_pop(l, 1);

	return result;
}


/**-------------------------------------------------------
*	\brief Entity Lua API
*///------------------------------------------------------

/**
*	\brief 检查栈上index索引处是否为sprite userdata
*	\return the sprite
*/
EntityPtr LuaContext::CheckEntity(lua_State*l, int index)
{
	return std::static_pointer_cast<Entity>(CheckUserdata(l, index, module_entity_name));
}

/**
*	\brief 返回栈上index索引处是否sprite
*	\return true 如果是sprite
*/
bool LuaContext::IsEntity(lua_State*l, int index)
{
	for (const auto& name : mEntityClassNames)
	{
		if (IsUserdata(l, index, name))
			return true;
	}
	return false;
}

/**
*	\brief 创建精灵entity:CreateSprite(name)
*/
int LuaContext::entity_api_create_sprite(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Entity& entity = *CheckEntity(l, 1);

		const std::string& name = LuaTools::CheckString(l, 2);
		std::string id = "";
		if (lua_isstring(l, 3))
			id = LuaTools::CheckString(l, 3);

		auto animate = entity.CreateAnimationSprite(name, id);
		PushAnimation(l, *animate);
		return 1;
	});
}

/**
*	\brief 返回当前entity类型 entity:GetType()
*/
int LuaContext::entity_api_get_type(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Entity& entity = *CheckEntity(l, 1);
		int type = entity.GetEntityType();
		lua_pushinteger(l, type);
		return 1;
	});
}

/**
*	\brief 返回当前entity所属的 entity:GetType()
*/
int LuaContext::entity_api_get_map(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Entity& entity = *CheckEntity(l, 1);
		auto& map = entity.GetMap();
		GetLuaContext(l).PushMap(l, map);
		return 1;
	});
}

/**
*	\brief 返回当前entity所属的 entity:GetSprite()
*/
int LuaContext::entity_api_get_sprite(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Entity& entity = *CheckEntity(l, 1);
		auto&sprites =  entity.GetSprites();
		if (sprites.size() > 0)
		{
			auto sprite = sprites[0].sprite;
			PushSprite(l, *sprite);
			return 1;
		}
		return 0;
	});
}

/**
*	\brief 返回当前entity类型 entity:GetType()
*/
int LuaContext::entity_api_get_game(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Entity& entity = *CheckEntity(l, 1);
		auto& game = entity.GetGame().GetSavegame();
		
		GetLuaContext(l).PushGame(l, game);
		return 1;
	});
}

/**-------------------------------------------------------
*	\brief Enemy Lua API
*///------------------------------------------------------

/**
*	\brief Entity执行TryHurt操作
*/
int LuaContext::entity_api_try_hurt(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Enemy& enemy = *std::static_pointer_cast<Enemy>(CheckUserdata(l, 1, module_enemy_name));
		int attack = LuaTools::CheckInt(l, 2);
		Entity& source = *CheckEntity(l, 3);

		EntityAttack atackType = static_cast<EntityAttack>(attack);
		enemy.TryHurt(atackType, source);
		return 0;
	});
}

/**-------------------------------------------------------
*	\brief Player Lua API
*///------------------------------------------------------

std::shared_ptr<Player> LuaContext::CheckPlayer(lua_State*l, int index)
{
	return std::static_pointer_cast<Player>(CheckUserdata(l, index, module_player_name));
}

bool LuaContext::IsPlyaer(lua_State*l, int index)
{
	return IsUserdata(l, index, module_player_name);
}

