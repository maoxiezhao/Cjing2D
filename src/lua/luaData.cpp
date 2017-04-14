#include "luaData.h"
#include "core\debug.h"

bool LuaData::ImportFromFile(const string & filename)
{
	lua_State* l = luaL_newstate();
	if (luaL_loadfile(l, filename.c_str()) != 0)
	{
		Debug::Error("Can't load lua data file:" + filename);
		lua_pop(l, 1);
		return false;
	}
	bool success = ImportFromLua(l);
	lua_close(l);
	return success;
}
