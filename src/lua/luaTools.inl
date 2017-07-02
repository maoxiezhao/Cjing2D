#include "luaTools.h"

namespace LuaTools {

template<typename Callable>
int  ExceptionBoundary(lua_State*l, Callable&& func)
{
	try
	{
		return func();
	}
	catch (const LuaException& ex)
	{

		luaL_error(l, ex.what());
	}
	catch (const std::exception&ex)
	{
		luaL_error(l, (string("Error:") + ex.what()).c_str());
	}

	return 0;
}

template<typename E>
E CheckEnum(lua_State * l, int index)
{
	return CheckEnum(l, index, EnumInfoTraits<E>::names);
}

template<typename E>
E CheckEnum(lua_State * l, int index, const std::map<E, string>& names)
{
	const string&name = LuaTools::CheckString(l, index);
	for (const auto& kvp : names)
	{
		if (kvp.second == name)
		{
			return kvp.first;
		}
	}
	string allowedNames;
	for (const auto&kvp : names)
	{
		allowedNames += "\"" + kvp.second + "\",";
	}
	allowedNames = allowedNames.substr(0, allowedNames.length() - 2);
	ArgError(l, index, std::string("Invailed name '") + name + ", allowed name are " + allowedNames);

	return E();
}


}