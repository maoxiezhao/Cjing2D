#include"luaTools.h"
#include<iostream>

namespace LuaTools
{
	bool CallFunction(lua_State*l, int arguments, int results, const string& functionName)
	{
		if (lua_pcall(l, arguments, results, 0) != 0)
		{
			std::cout << "the function:" << functionName << "called failed." << std::endl;
			lua_pop(l, 1);
			return false;
		}
		return true;;
	}

	int GetPositiveIndex(lua_State * l, int index)
	{
		int positiveIndex = index;
		if (index < 0 && index >= -lua_gettop(l))
			positiveIndex += lua_gettop(l) + index + 1;
		
		return positiveIndex;
	}




}
