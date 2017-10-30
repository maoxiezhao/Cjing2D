#include "gameResources.h"

const string EnumInfoTraits<GameResourceType>::prettyName = "resourceType";
const EnumInfo<GameResourceType>::nameType EnumInfoTraits<GameResourceType>::names = {
	{ GameResourceType::MAP, "map" },
	{ GameResourceType::ITEM, "item"}
};

GameResource::GameResource()
{
}

/**
*	\brief 获取指定资源类型的描述信息
*/
GameResource::GameResourceInfos & GameResource::GetGameResourcesInfos(GameResourceType type)
{
	auto& kvp = mResourceInfos.find(type);
	if (kvp == mResourceInfos.end())
	{
		Debug::Error("Failed to get game resource.");
	}
	return kvp->second;
}

/**
*	\brief 获取指定资源类型的描述信息
*/
const GameResource::GameResourceInfos & GameResource::GetGameResourcesInfos(GameResourceType type) const
{
	const auto& kvp = mResourceInfos.find(type);
	if (kvp == mResourceInfos.end())
	{
		Debug::Error("Failed to get game resource.");
	}
	return kvp->second;
}

/**
*	\brief 添加指定资源类型的描述信息
*/
void GameResource::AddGameResourceInfo(GameResourceType type, const std::string id)
{
	mResourceInfos[type].push_back(id);
}

/**
*	\brief 删除指定资源类型的描述信息
*/
void GameResource::RemoveResourceInfo(GameResourceType type, const std::string id)
{
	auto& kvp = mResourceInfos.find(type);
	if (kvp == mResourceInfos.end())
	{
		Debug::Error("Failed to get game resource.");
	}
	auto& resourceInfo = kvp->second;
	auto itor = std::find(resourceInfo.begin(), resourceInfo.end(), id);
	if (itor == resourceInfo.end())
	{
		Debug::Warning("Failed to remove resourceInf:" + id + 
			", the resource id is non-exists.");
	}
	resourceInfo.erase(itor);
}

/**
*	\brief 返回每个类型资源目录下是否存在id的资源
*/
bool GameResource::ExistsResource(GameResourceType type, const std::string id)
{
	auto& kvp = mResourceInfos.find(type);
	if (kvp == mResourceInfos.end())
	{
		return false;
	}
	auto& resourceInfo = kvp->second;
	return std::find(resourceInfo.begin(), resourceInfo.end(), id) != resourceInfo.end();
}

bool GameResource::ImportFromLua(lua_State * l)
{
	lua_pushlightuserdata(l, this);
	lua_setfield(l, LUA_REGISTRYINDEX, "resourceData");

	// 注册资源处理函数
	lua_register(l, "resource", LuaLoadResourceData);
	for (const auto& kvp : EnumInfoTraits<GameResourceType>::names)
	{	
		// 这里不使用类似entityType的闭包形式来解析各个数据
		// 直接执行function,减少全局污染
		const std::string& resourceStr = kvp.second;
		const std::string functionStr = string("function ") + resourceStr + " (t) resource('" +
			resourceStr + "', t) end";
		luaL_dostring(l, functionStr.c_str());
	}

	// do function
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		Debug::Error("Failed to load animation datafile.");
		lua_pop(l, 1);	
		return false;
	}

	return true;
}

GameResource & GameResource::GetGameResource()
{
	static GameResource gameResource;
	return gameResource;
}

/**
*	\brief 解析各个资源类型
*
*	该函数由ImportFromLua中luaL_doString间接调用
*	function  ResoureType(table)resource(table, t) end
*/
int GameResource::LuaLoadResourceData(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		GameResourceType type = LuaTools::CheckEnum<GameResourceType>(l, 1);
		GameResource::LuaCheckDefualtType(l, 2, type);

		return 0;
	});
}

/**
*	\brief 默认的解析函数
*
*	该函数会解析id并将数据存储在resoureInfos
*/
int GameResource::LuaCheckDefualtType(lua_State * l, int index, GameResourceType type)
{
	lua_getfield(l, LUA_REGISTRYINDEX, "resourceData");
	GameResource& gameResource = *static_cast<GameResource*>(lua_touserdata(l, -1));
	lua_pop(l, 1);

	index = LuaTools::GetPositiveIndex(l, index);
	LuaTools::CheckType(l, index, LUA_TTABLE);

	const std::string& id = LuaTools::CheckFieldString(l, index, "id");
	gameResource.AddGameResourceInfo(type, id);

	return 0;
}

int GameResource::LuaCheckItemType(lua_State * l, int index)
{
	return 0;
}
