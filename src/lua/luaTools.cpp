#include"luaTools.h"
#include"luaException.h"
#include"core\debug.h"
#include"core\logger.h"
#include<iostream>
#include<sstream>

namespace LuaTools
{
	LuaRef CreateRef(lua_State * l)
	{
		return LuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
	}
	LuaRef CreateRef(lua_State * l, int index)
	{
		lua_pushvalue(l, index);
		return LuaRef(l,luaL_ref(l,LUA_REGISTRYINDEX));
	}

	bool CallFunction(lua_State*l, int arguments, int results, const string& functionName)
	{
		if (lua_pcall(l, arguments, results, 0) != 0)
		{
			std::string errMsg = std::string("In ") + functionName + ": " + lua_tostring(l, -1) + "\n";
			luaL_traceback(l, l, NULL, 1);
			errMsg += lua_tostring(l, -1);
			Debug::Error(errMsg);
			lua_pop(l, 2);
			return false;
		}
		return true;;
	}

	int GetPositiveIndex(lua_State * l, int index)
	{
		int positiveIndex = index;
		if (index < 0 && index >= -lua_gettop(l))
			positiveIndex = lua_gettop(l) + index + 1;
		
		return positiveIndex;
	}

	void CheckType(lua_State * l, int index, int exceptedType)
	{
		if (lua_type(l, index) != exceptedType)
			ArgError(l, index, string("excepted") + string(luaL_typename(l, exceptedType)) +
				" but get" + luaL_typename(l, index));
	}

	void CheckAny(lua_State * l, int index)
	{
		if (lua_type(l, index) == LUA_TNIL)
			ArgError(l, index, string("excepted no nil,") +
				" but get" + luaL_typename(l, index));
	}

	bool CheckBoolean(lua_State * l, int index)
	{
		if (!lua_isboolean(l, index))
			ArgError(l, index, string("Excepted:boolean,got ") + luaL_typename(l, index));

		return (bool)lua_toboolean(l, index);
	}

	bool OptBoolean(lua_State * l, int index, bool defaultValue)
	{
		if (!lua_isboolean(l, index))
		{
			return defaultValue;
		}
		return CheckBoolean(l, index);
	}

	int CheckInt(lua_State * l, int index)
	{
		if (!lua_isnumber(l, index))
			ArgError(l, index, string("Excepted:integer,got ") + luaL_typename(l, index));

		return (int)lua_tointeger(l,index);
	}

	int CheckIntByDefault(lua_State * l, int index, int defaultValue)
	{
		if (!lua_isinteger(l, index))
		{
			return defaultValue;
		}
		return CheckInt(l, index);
	}

	double CheckNumber(lua_State * l, int index)
	{
		if (!lua_isnumber(l, index))
			ArgError(l, index, string("Excepted:Number,got ") + luaL_typename(l, index));

		return (double)lua_tonumber(l, index);
	}

	double CheckFieldNumber(lua_State * l, int tableIndex, const string & name)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (!lua_isnumber(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:Number type,got ") + luaL_typename(l, -1));
		}
		double value = CheckNumber(l, -1);
		lua_pop(l, 1);
		return value;
	}

	float CheckFloat(lua_State * l, int index)
	{
		if (!lua_isnumber(l, index))
			ArgError(l, index, string("Excepted:float,got ") + luaL_typename(l, index));

		return (float)lua_tonumber(l, index);
	}

	string CheckString(lua_State * l, int index)
	{
		if (!lua_isstring(l, index))
			ArgError(l, index, string("Excepted:string,got ") + luaL_typename(l, index));
		return lua_tostring(l, index);
	}

	string CheckStringByDefault(lua_State * l, int index, const std::string & defaultValue)
	{
		if (!lua_isstring(l, index))
		{
			return defaultValue;
		}
		return CheckString(l, index);
	}

	bool CheckFieldBool(lua_State*l, int tableIndex, const string& name)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (!lua_isboolean(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:color type,got ") + luaL_typename(l, -1));
		}
		bool value = CheckBoolean(l, -1);
		lua_pop(l, 1);
		return value;
	}

	bool CheckFieldBoolByDefault(lua_State*l, int tableIndex, const string& name, bool defaultValue)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (lua_isnil(l, -1))
		{
			lua_pop(l, 1);	// pop nil
			return defaultValue;
		}

		if (!lua_isboolean(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:color type,got ") + luaL_typename(l, -1));
		}
		bool value = CheckBoolean(l, -1);
		lua_pop(l, 1);
		return value;
	}

	LuaRef CheckFunction(lua_State * l, int index)
	{
		CheckType(l, index, LUA_TFUNCTION);
		return CreateRef(l, index);;
	}

	LuaRef OptFunction(lua_State * l, int index)
	{
		if (lua_isnoneornil(l, index))
		{
			return LuaRef();
		}
		return CheckFunction(l, index);
	}

	bool IsColor(lua_State*l, int index)
	{
		index = GetPositiveIndex(l, index);
		if (lua_type(l, index) != LUA_TTABLE)
		{
			return false;
		}

		lua_rawgeti(l, index, 1);
		lua_rawgeti(l, index, 2);
		lua_rawgeti(l, index, 3);
		lua_rawgeti(l, index, 4);
		bool result = lua_isnumber(l, -4) &&
					  lua_isnumber(l, -3) &&
					  lua_isnumber(l, -2) &&
					  lua_isnumber(l, -1);
		lua_pop(l, 4);
		return result;
	}

	Color4B CheckColor(lua_State*l, int index)
	{
		index = GetPositiveIndex(l, index);
		CheckType(l, index, LUA_TTABLE);

		lua_rawgeti(l, index, 1);
		lua_rawgeti(l, index, 2);
		lua_rawgeti(l, index, 3);
		lua_rawgeti(l, index, 4);
		Color4B color(				// {r g b a}
			(GLubyte)CheckInt(l, -4),
			(GLubyte)CheckInt(l, -3),
			(GLubyte)CheckInt(l, -2),
			(GLubyte)CheckInt(l, -1)
		);
		lua_pop(l, 4);
		return color;
	}

	Color4B CheckFieldColor(lua_State*l, int tableIndex, const string& name)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (!IsColor(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:color type,got ") + luaL_typename(l, -1));
		}
		const Color4B& color = CheckColor(l, -1);
		lua_pop(l, 1);
		return color;
	}

	Color4B CheckFieldColorByDefault(lua_State*l, int tableIndex, const string&name, const Color4B defaultValue)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (lua_isnil(l, -1))
		{
			lua_pop(l, 1);	// pop nil
			return defaultValue;
		}

		if (!IsColor(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:color type,got ") + luaL_typename(l, -1));
		}

		const Color4B& color = CheckColor(l, -1);
		lua_pop(l, 1);
		return color;
	}

	bool IsPoint2(lua_State * l, int index)
	{
		index = GetPositiveIndex(l, index);
		if (lua_type(l, index) != LUA_TTABLE)
		{
			return false;
		}

		lua_rawgeti(l, index, 1);
		lua_rawgeti(l, index, 2);
		bool result = lua_isnumber(l, -2) && lua_isnumber(l, -1);
		lua_pop(l, 2);
		return result;
	}

	Point2 CheckPoint2(lua_State * l, int index)
	{
		index = GetPositiveIndex(l, index);
		CheckType(l, index, LUA_TTABLE);

		lua_rawgeti(l, index, 1);
		lua_rawgeti(l, index, 2);
		Point2 point(CheckInt(l, -2),
			CheckInt(l, -1));
		lua_pop(l, 2);
		return point;
	}

	Point2 CheckFieldPoint2(lua_State * l, int tableIndex, const string & name)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (!IsPoint2(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:Point2 type,got ") + luaL_typename(l, -1));
		}
		const Point2& point = CheckPoint2(l, -1);
		lua_pop(l, 1);
		return point;
	}

	Point2 CheckFieldPoint2ByDefault(lua_State * l, int tableIndex, const string & name, const Point2 defaultValue)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (lua_isnil(l, -1))
		{
			lua_pop(l, 1);	// pop nil
			return defaultValue;
		}

		if (!IsPoint2(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:Point2 type,got ") + luaL_typename(l, -1));
		}

		const Point2& point = CheckPoint2(l, -1);
		lua_pop(l, 1);
		return point;
	}

	/**
	*	\brief 获取table中名字为name的int值
	*/
	int CheckFieldInt(lua_State*l, int tableIndex,const string& name) 
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (!lua_isnumber(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:integer,got ") + luaL_typename(l, -1));
		}
		int value = (int)lua_tointeger(l, -1);
		lua_pop(l, 1);
		return value;
	}

	/**
	*	\brief 获取table名字为name的string值
	*/
	string CheckFieldString(lua_State*l, int tableIndex, const string&name)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (!lua_isstring(l,-1))
		{
			ArgError(l, tableIndex, string("Excepted:string,got ") + luaL_typename(l, -1));
		}
		const string& value = lua_tostring(l, -1);
		lua_pop(l, 1);
		return value;
	}

	string CheckFieldStringByDefault(lua_State*l, int tableIndex, const string& name, const string& defaultString)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (lua_isnil(l, -1))
		{
			lua_pop(l, 1); // pop nil
			return defaultString;
		}

		if (!lua_isstring(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:string,got ") + luaL_typename(l, -1));
		}
		const string& value = lua_tostring(l, -1);
		lua_pop(l, 1);
		return value;
	}

	/**
	*	\brief 获取table中名字为name的int值,如果值不存在，则返回默认值
	*/
	int CheckFieldIntByDefault(lua_State * l, int tableIndex, const string & name, int defaultValue)
	{
		lua_getfield(l, tableIndex, name.c_str());
		if (lua_isnil(l,-1))
		{
			lua_pop(l, 1);
			return defaultValue;
		}

		if (!lua_isnumber(l, -1))
		{
			ArgError(l, tableIndex, string("Excepted:integer,got ") + luaL_typename(l, -1));
		}
		int value = (int)lua_tointeger(l, -1);
		lua_pop(l, 1);
		return value;
	}

	/**
	*	\brief 代替Lua_error抛出luaException异常.
	*/
	void Error(lua_State * l, const string & message)
	{
		std::string msg(message);
		luaL_traceback(l, l, NULL, 1);
		msg += lua_tostring(l, -1);

		throw LuaException(l, msg);
	}

	/**
	*	\brief 抛出luaException调用的第 index 个参数的问题
	*
	*	该函数不返回.
	*/
	void ArgError(lua_State * l, int index, const string & message)
	{
		std::ostringstream oss;
		lua_Debug info;
		if (!lua_getstack(l, 0, &info))		// 获取当前运行函数的活动记录
		{
			// ??????
			oss << "Bad argument #" << index << "(" << message << ")";
			Error(l, oss.str());
		}
			
		lua_getinfo(l, "n", &info);			// 填充name域，即当前函数的名字域
		if (string(info.namewhat) == "method")
		{
			// ?????
			oss << "Calling:" << info.name << " failed. (" << message << ")";
			Error(l, oss.str());
		}

		if (info.name == nullptr)
			info.name = "?";

		oss << "Bad argument #" << index << " to " <<  info.name << "(" << message << ")";
		Error(l,oss.str());
	}

	void PrintLuaStack(lua_State * l)
	{
		int stackSize = lua_gettop(l);
		std::ostringstream oss;
		oss << endl;
		for (int i = 1; i <= stackSize; i++)
		{
			switch (lua_type(l, i))
			{
			case LUA_TSTRING:
				oss << "\"" << lua_tostring(l, i) << "\"";
				break;
			case LUA_TBOOLEAN:
				oss << (lua_toboolean(l, i) ? "True" : "False");
				break;
			case LUA_TNUMBER:
				oss << (lua_tonumber(l, i));
				break;
			default:
				oss << lua_typename(l, lua_type(l, i));
				break;
			}
			oss << endl;
		}
		Logger::Debug(oss.str());
	}

	void PrintInIndex(lua_State * l, int index)
	{
		index = LuaTools::GetPositiveIndex(l, index);
		std::ostringstream oss;
		switch (lua_type(l, index))
		{
		case LUA_TSTRING:
			oss << "\"" << lua_tostring(l, index) << "\"";
			break;
		case LUA_TBOOLEAN:
			oss << (lua_toboolean(l, index) ? "True" : "False");
			break;
		case LUA_TNUMBER:
			oss << (lua_tonumber(l, index));
			break;
		default:
			oss << lua_typename(l, lua_type(l, index));
			break;
		}
		Logger::Debug(oss.str());
	}

	int GetCallDepth(lua_State * l)
	{
		int depth = 0;
		lua_Debug info;
		for (; lua_getstack(l, depth + 1, &info) != 0; depth++);
		return depth;
	}




}
