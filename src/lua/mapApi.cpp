#include"lua\luaContext.h"
#include"game\map.h"
#include"core\fileData.h"

const string LuaContext::module_map_name = "Map";

void LuaContext::RegisterMapModule()
{
	static const luaL_Reg methods[] = {
		{ nullptr,nullptr }
	};

	static const luaL_Reg metamethos[] = {
		{ "__newindex", userdata_meta_newindex },
		{ "__index", userdata_meta_index },
		{ "__gc", userdata_meta_gc },
		{ nullptr, nullptr }
	};
	RegisterType(l, module_map_name, nullptr, methods, metamethos);
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

/**
*	\brief 执行地图
*
*	读取mapID.lua文件，同时考虑到map中entity的处理事件如果
*   发置在lua文件中
*/
void LuaContext::RunMap(Map & map)
{
	string& mapFilePath = "maps/" + map.GetMapID();

	bool result = LoadFile(l, mapFilePath);
	if (result == false)
	{
		Debug::Error("Faild to load map lua file '" + map.GetMapID() + "'");
	}

	// 压入当前map作为参数
	PushMap(l, map);
	LuaTools::CallFunction(l, 1, 0, map.GetMapID().c_str());

	// 执行onStarted
	OnMapStart(map);
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
