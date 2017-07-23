#include"lua\luaContext.h"

const string LuaContext::module_map_name = "Map";

void LuaContext::OnMapStart()
{
}

void LuaContext::OnMapUpdate()
{
}

void LuaContext::OnMapFinish()
{
}

void LuaContext::OnMapDraw()
{
}

bool LuaContext::OnMapInput(const InputEvent& event)
{
	return false;
}