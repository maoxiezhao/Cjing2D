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
		Debug::Error(ex.what());
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

template<typename E>
E CheckFiledEnum(lua_State * l, int tableIndex, const string & name)
{
	lua_getfield(l, tableIndex, name.c_str());
	if (!lua_isstring(l, -1))
	{
		ArgError(l, tableIndex, string("Excepted:String type,got ") + luaL_typename(l, -1));
	}
	E value = CheckEnum<E>(l, -1);
	lua_pop(l, 1);
	return value;
}


/**
*	\brief 用于直接创建userdata中的数据
*/
template<typename T>
struct LuaBindingUserdata
{
	static void PushMetatable(lua_State*l, const std::string& name)
	{
		luaL_getmetatable(l, name.c_str());
		if (lua_istable(l, 1))
		{
			return;
		}
		lua_pop(l, 1);
		luaL_newmetatable(l, name.c_str());
		lua_pushcfunction(l, LuaUserdataGC);
		lua_setfield(l, -2, "__gc");
	}

	template<typename... Args>
	static T* Create(lua_State*l, const std::string& name, Args&&... args)
	{
		T* obj = new(lua_newuserdata(l, sizeof(T)))T(std::forward<Args>(args)...);

		PushMetatable(l, name);
		lua_setmetatable(l, -2);

		return obj;
	}

	// 临时gc方法
	static int LuaUserdataGC(lua_State* l)
	{
		return LuaTools::ExceptionBoundary(l, [&] {
			if (lua_gettop(l) == 0)
			{
				LuaTools::Error(l, "Gc function called without self.");
				return 0;
			}

			T* obj = static_cast<T*>(lua_touserdata(l, 1));
			if (obj == nullptr)
			{
				LuaTools::Error(l, "The userdata is null when called gc.");
				return 0;
			}

			obj->~T();
			return 0;
		});
	}
};


}