#include"lua\luaContext.h"

const string LuaContext::module_movement_name = "Movement";
const string LuaContext::module_straight_movment_name = "StraightMovement";

void LuaContext::RegisterMovementModule()
{
	static const luaL_Reg function[] = {
		{ "create", sprite_api_create },
		{ nullptr,nullptr }
	};

	static const luaL_Reg methods[] = {
		{ nullptr, nullptr }
	};

	static const luaL_Reg metamethods[] = {
		{ "__gc", userdata_meta_gc },
		{ nullptr, nullptr }
	};

	RegisterType(module_movement_name, function, methods, metamethods);
}

int LuaContext::movement_api_create(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
			
		return 0;
	});
}