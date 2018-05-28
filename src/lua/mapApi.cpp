#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"game\map.h"
#include"core\fileData.h"
#include"entity\entityInfo.h"

const string LuaContext::module_map_name = "Map";
const string LuaContext::module_room_name = "MapRoom";

/**
*	\brief 用来加载地图的Lua脚本，使用脚本加载而不是
*	系统C++的LoadFile加载，是为了加载地图的同时，设置
*	一些必须的环境
*/
const std::string LoadMapScript =
"local func, map, path = ...\n"
"local function traced_pcall(func, ...)\n"
"local function msgHander(msg)\n"
"return debug.traceback(msg, 2)\n"
"end\n"
"return xpcall(func, msgHander, ...)\n"
"end\n"
"if not CLIENT_LUA_DEBUG then\n"
"traced_pcall = pcall\n"
"end\n"
"local env = {}\n"
"setmetatable(env, { __index = function(t, k)\n"
"local v = func(k)\n"
"if v then return v end\n"
"local v = _ENV[k]\n"
"if v then return v end\n"
"end })\n"
"traced_pcall(SystemDoFile, path .. '.lua', env, map)\n"
;

void LuaContext::RegisterMapModule()
{
	// Map base class
	LuaBindClass<Map> mapClass(l, module_map_name);
	mapClass.AddDefaultMetaFunction();
	mapClass.AddMethod("SetBackground", map_api_set_background);

	// Room base class
	LuaBindClass<MapRoom> mapRoomClass(l, module_room_name);
	mapRoomClass.AddDefaultMetaFunction();
	mapRoomClass.AddMethod("GetMap", room_api_get_map);
	mapRoomClass.AddMethod("GetPos", &MapRoom::GetPos);
	mapRoomClass.AddMethod("GetSize", &MapRoom::GetSize);

	/** 创建各个entity的Create函数 */
	luaL_getmetatable(l, module_map_name.c_str());
	for (const auto& kvp : EnumInfoTraits<EntityType>::names)
	{
		auto entityType = kvp.first;
		auto entityName = kvp.second;
		std::string funcName = "Create_" + entityName;
		lua_pushstring(l, entityName.c_str());
		lua_pushcclosure(l, map_api_create_entity, 1);
		lua_setfield(l, -2, funcName.c_str());
	}
	lua_pop(l, 1);
}

void LuaContext::PushMap(lua_State*l, Map& map)
{
	PushUserdata(l, map);
}

std::shared_ptr<Map> LuaContext::CheckMap(lua_State*l, int index)
{
	return std::dynamic_pointer_cast<Map>(CheckUserdata(l, index, module_map_name));
}

bool LuaContext::IsMap(lua_State * l, int index)
{
	return IsUserdata(l, index, module_map_name);
}

void LuaContext::RunRoom(MapRoom & room)
{
	// 地图__index处理
	if (luaL_loadstring(l, LoadMapScript.c_str()) != 0) {
		Debug::Error("Faild to set map env. file '" + room.GetMapID() + "'");
		return;
	}
	PushMap(l, room.GetMap());
	lua_pushcclosure(l, map_api_get_entity_global, 1);
	// envScripts func
	PushUserdata(l, room);
	// envScripts func map
	string& mapFilePath = room.GetMapPath();
	lua_pushstring(l, mapFilePath.c_str());
	// envScripts func map mapPath						
	LuaTools::CallFunction(l, 3, 0, "Set map env.");
	// 执行OnStart
	CallFunctionWithUserdata(room, "OnStarted");
}

void LuaContext::LeaveRoom(MapRoom & room)
{
	CallFunctionWithUserdata(room, "OnFinished");
}

/**
*	\brief 执行地图
*
*	读取mapID.lua文件，同时考虑到map中entity的处理事件如果
*   发置在lua文件中
*/
void LuaContext::RunMap(Map & map)
{
	// 地图__index处理
	if (luaL_loadstring(l, LoadMapScript.c_str()) != 0) {
		Debug::Error("Faild to set map env. file '" + map.GetMapID() + "'");
		return;
	}
	PushMap(l, map);
	lua_pushcclosure(l, map_api_get_entity_global, 1);
						// envScripts func
	PushMap(l, map);
						// envScripts func map
	string& mapFilePath = "maps/" + map.GetMapID() + "/mapInfo";
	lua_pushstring(l, mapFilePath.c_str());
						// envScripts func map mapPath						
	LuaTools::CallFunction(l, 3, 0, "Set map env.");

	// 执行map Enter
	DoFireLuaSystemEvent(SystemLuaEvent::EVENT_GAME_MAP_ENTER, 
		[&](lua_State*l) {
		PushMap(l, map);
		return 1;
	});

	// 执行onStarted
	OnMapStart(map);
}

/**
*	\brief 退出当前地图
*/
void LuaContext::LeaveMap(Map & map)
{
	// Do OnFinished
	OnMapFinish(map);

	// 执行map Leave
	DoFireLuaSystemEvent(SystemLuaEvent::EVENT_GAME_MAP_LEAVE, nullptr);
}

void LuaContext::OnMapStart(Map & map)
{
	PushMap(l, map);
	OnStart();
	lua_pop(l, 1);
}

void LuaContext::OnMapUpdate(Map & map)
{
	PushMap(l, map);
	OnUpdate();
	lua_pop(l, 1);
}

void LuaContext::OnMapFinish(Map & map)
{
	PushMap(l, map);
	OnFinish();
	lua_pop(l, 1);
}

void LuaContext::OnMapDraw(Map & map)
{
	PushMap(l, map);
	OnDraw();
	lua_pop(l, 1);
}

bool LuaContext::OnMapInput(Map & map, const InputEvent & event)
{
	return false;
}

/**
*	\brief 实现game:setLife(life)
*/
int LuaContext::map_api_set_background(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& map = *CheckMap(l, 1);
		const std::string& filePath = LuaTools::CheckString(l, 2);
		auto background = std::make_shared<Sprite>(filePath);
		map.SetBackground(background);

		return 0;
	});
}

/**
*	\brief 实现game:setLife(life)
*/
int LuaContext::map_api_create_entity(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Map& map = *CheckMap(l, 1);
		const std::string& typeName = LuaTools::CheckString(l, lua_upvalueindex(1));
		EntityType type = StringToEnum<EntityType>(typeName);
		const EntityData& entityData = EntityData::CheckEntityData(l, 2, type);
		LuaRef initCallback;
		if (lua_isfunction(l, 3))
			initCallback = LuaTools::CheckFunction(l, 3);

		GetLuaContext(l).CreateEntity(entityData, map, initCallback);
		return 1;
	});
}
 

/**
*	\brief 用于实现在加载map时获取对应的Entity或者Global
*/
int LuaContext::map_api_get_entity_global(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		return 0;
	});
}

/**
*	\brief 从room中获取map实例, room:GetMap()
*/
int LuaContext::room_api_get_map(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& room = *std::dynamic_pointer_cast<MapRoom>(
			CheckUserdata(l, 1, module_room_name));
		auto& map = room.GetMap();
		PushMap(l, map);
		return 1;
	});
}

