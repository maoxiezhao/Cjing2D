#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"game\map.h"
#include"core\fileData.h"
#include"entity\entityInfo.h"

const string LuaContext::module_map_name = "Map";
const string LuaContext::module_room_name = "MapRoom";

/**
*	\brief �������ص�ͼ��Lua�ű���ʹ�ýű����ض�����
*	ϵͳC++��LoadFile���أ���Ϊ�˼��ص�ͼ��ͬʱ������
*	һЩ����Ļ���
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

	/** ��������entity��Create���� */
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
	// ��ͼ__index����
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
	// ִ��OnStart
	CallFunctionWithUserdata(room, "OnStarted");
}

void LuaContext::LeaveRoom(MapRoom & room)
{
	CallFunctionWithUserdata(room, "OnFinished");
}

/**
*	\brief ִ�е�ͼ
*
*	��ȡmapID.lua�ļ���ͬʱ���ǵ�map��entity�Ĵ����¼����
*   ������lua�ļ���
*/
void LuaContext::RunMap(Map & map)
{
	// ��ͼ__index����
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

	// ִ��map Enter
	DoFireLuaSystemEvent(SystemLuaEvent::EVENT_GAME_MAP_ENTER, 
		[&](lua_State*l) {
		PushMap(l, map);
		return 1;
	});

	// ִ��onStarted
	OnMapStart(map);
}

/**
*	\brief �˳���ǰ��ͼ
*/
void LuaContext::LeaveMap(Map & map)
{
	// Do OnFinished
	OnMapFinish(map);

	// ִ��map Leave
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
*	\brief ʵ��game:setLife(life)
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
*	\brief ʵ��game:setLife(life)
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
*	\brief ����ʵ���ڼ���mapʱ��ȡ��Ӧ��Entity����Global
*/
int LuaContext::map_api_get_entity_global(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		return 0;
	});
}

/**
*	\brief ��room�л�ȡmapʵ��, room:GetMap()
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

