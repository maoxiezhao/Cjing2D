#include"lua\luaContext.h"
#include"core\fileData.h"
#include"game\map.h"
#include"game\mapData.h"
#include"entity\tile.h"
#include"entity\entities.h"
#include"entity\tileset.h"

const string LuaContext::module_entity_name = "Entity";

void LuaContext::RegisterEntityModule()
{
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
	//RegisterType(module_entity_name, functions, methods, metamethos);
}

/**
*	\brief 所有的entity的lua构造函数
*/
std::map<EntityType, lua_CFunction> LuaContext::mEntitityCreaters = 
{
	{ EntityType::TITLE, entity_api_create_title},
	{ EntityType::DESTIMATION, entity_api_create_destimation },
	{ EntityType::DYNAMIC_TITLE, entity_api_create_dynamic_title },
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
		tileInfo.mPattern = &tileset.GetTilePattern(tileInfo.mPatternID);
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
*	\brief 创建对应的entity
*/
bool LuaContext::CreateEntity(const EntityData& entityData, Map& map)
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
	return LuaTools::CallFunction(l, 2, 1, "create entity");
}
