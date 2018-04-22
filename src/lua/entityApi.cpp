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


const std::string LuaContext::module_entity_name = "Entity";
const std::string LuaContext::module_enemy_name = "Enemy";

void LuaContext::RegisterEntityModule()
{
	// entity base
	static const luaL_Reg functions[] = {
		{ nullptr,nullptr }
	};	
	static const luaL_Reg methods[] = {
		{ nullptr,nullptr }
	};
	static const luaL_Reg metamethos[] = {
		{ "__newindex", userdata_meta_newindex },
		{ "__index", userdata_meta_index },
		{ "__gc", userdata_meta_gc },
		{ nullptr, nullptr }
	};
//	RegisterType(l, module_entity_name, functions, methods, metamethos);

	// entity enemy
	static const luaL_Reg enemy_methods[] = {
		{ nullptr,nullptr }
	};
//	RegisterType(l, module_enemy_name, nullptr, enemy_methods, metamethos);
	// Enemy
	LuaBindClass<Enemy> bindClass(l, "Enemy");
	bindClass.AddMetaFunction("__gc", LuaContext::userdata_meta_gc);
	bindClass.AddMetaFunction("__index", LuaContext::userdata_meta_index);
	bindClass.AddMetaFunction("__newindex", LuaContext::userdata_meta_newindex);
	bindClass.AddMethod("GetName", &Enemy::GetName);
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
	{ EntityType::ENEMEY, entity_api_create_enemy }
};

/**
*	\brief entity_title的创建函数
*/
int LuaContext::entity_api_create_title(lua_State* l) 
{
	return LuaTools::ExceptionBoundary(l, [&] {
		//GetLuaContext(l).PrintLuaStack(l);
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
		const Map& map = *CheckMap(l, 1);
		const EntityData& entityData = *static_cast<EntityData*>(lua_touserdata(l, 2));

		lua_pushboolean(l, true);
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
			entityData.GetName(), 
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
			entityData.GetLayer(), 
			entityData.GetPos());

		enemy->SetDirection(static_cast<Direction4>(
			entityData.GetValueInteger("direction")));
		
		if (map.IsStarted())
		{
			PushUserdata(l, *enemy);
						// enemy
			if (lua_isfunction(l, 3))
			{			// func enemy
				LuaTools::CallFunction(l, 1, 0, "enemy init");
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
*	\brief 创建对应的entity
*
*	其中map和该userdata会作为参数传入, initCallback会在实体创建完后
*	地图创建之前调用，一般可用于特殊的初始化需要
*/
bool LuaContext::CreateEntity(const EntityData& entityData, Map& map, LuaRef& initCallback)
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
	return LuaTools::CallFunction(l, argNum, 1, "create entity");
}
