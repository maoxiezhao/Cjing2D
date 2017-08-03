#include"lua\luaContext.h"

const string LuaContext::module_game_name = "cjing.Game";

void LuaContext::RegisterGameModule()
{
	static const luaL_Reg functions[] = {
		{nullptr,nullptr}
	};

	static const luaL_Reg methods[] = {
		{ nullptr,nullptr }
	};

	static const luaL_Reg metamethos[] = {
		{nullptr,nullptr}
	};

}

void LuaContext::OnGameStart()
{
}

void LuaContext::OnGameUpdate()
{
}

void LuaContext::OnGameFinish()
{
}

void LuaContext::OnGameDraw()
{
}

bool LuaContext::OnGameInput(const InputEvent & event)
{
	return false;
}
