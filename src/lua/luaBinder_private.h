#pragma once

#include"lua\luaTools.h"
#include"lua\luaData.h"
#include"lua\luaContext.h"
#include"utils\typeSet.h"
#include"utils\size.h"

#include<string>

/**
*	\brief 函数调度器
*/
namespace Implemention
{
	/** 当前已经注册的Lua Wrapper类,一般仅用于常用Util类 */
	using WrapperSet = util::typeset<
		std::string,
		Size,
		Point2
	>;

	/**
	*	\brief lua到C++数据解析器
	*/
	namespace UnWrapperValue
	{
		/** 整型 */
		template<typename T>
		inline typename std::enable_if<std::is_integral<T>::value, T>::type
			UnWraper(lua_State*l, int index) { return static_cast<T>(LuaTools::CheckInt(l, index)); }
		/** 浮点型 */
		template<typename T>
		inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
			UnWraper(lua_State*l, int index) { return static_cast<T>(LuaTools::CheckNumber(l, index)); }
		/** boolean */
		template<typename T>
		inline typename std::enable_if<std::is_same<T, bool>::value, T>::type
			UnWraper(lua_State*l, int index) { return static_cast<T>(LuaTools::CheckBoolean(l, index)); }
		/** string */
		template<typename T>
		inline typename std::enable_if<std::is_same<T, std::string>::value, T>::type
			UnWraper(lua_State*l, int index) { return static_cast<T>(LuaTools::CheckString(l, index)); }
		/** size */
		template<typename T>
		inline typename std::enable_if<std::is_same<T, Size>::value, T>::type
			UnWraper(lua_State*l, int index) {
			LuaTools::CheckType(l, index, LUA_TTABLE);
			lua_rawgeti(l, index, 1);
			lua_rawgeti(l, index, 2);
			Size size(LuaTools::CheckInt(l, -2),
				LuaTools::CheckInt(l, -1));
			lua_pop(l, 2);
			return static_cast<T>(size);
		}
		/** Point2 */
		template<typename T>
		inline typename std::enable_if<std::is_same<T, Point2>::value, T>::type
			UnWraper(lua_State*l, int index) {
			Point2 point = LuaTools::CheckPoint2(l, index);
			return static_cast<T>(point);
		}
		/** userdata */
		template<typename T>
		inline typename std::enable_if<std::is_class<T>::value && !util::typeset_exist<WrapperSet, T>::value, T>::type
			UnWraper(lua_State*l, int index) { return static_cast<T>(T::LuaUnWraper(l, index)); }	// 需要该类提供LuaUnWraper接口
	}

	/**
	*	\brief C++到lua数据解析器
	*/
	namespace WrapperValue
	{
		/** 整型 */
		template<typename T>
		inline std::enable_if_t<std::is_integral<T>::value>
			Wraper(lua_State*l, T value)
		{
			lua_pushinteger(l, value);
		}
		/** 浮点型 */
		template<typename T>
		inline std::enable_if_t<std::is_floating_point<T>::value>
			Wraper(lua_State*l, T value)
		{
			lua_pushnumber(l, value);
		}
		/** boolean */
		inline void Wraper(lua_State*l, bool value)
		{
			lua_pushboolean(l, value);
		}
		/** string */
		inline void Wraper(lua_State*l, const std::string& value)
		{
			lua_pushstring(l, value.c_str());
		}
		/** size */
		inline void Wraper(lua_State*l, const Size& value)
		{
			lua_newtable(l);
			lua_pushinteger(l, value.width);
			lua_rawseti(l, -1, 1);
			lua_pushinteger(l, value.height);
			lua_rawseti(l, -1, 2);
		}

		/** userdata 需要该类提供LuaWraper接口*/																
		template<typename T>
		inline std::enable_if_t<std::is_class<T>::value &&
		 !util::typeset_exist<WrapperSet, T>::value >
			Wraper(lua_State*l, T value)
		{
			T::LuaWraper(l);
		//	lua_pushinteger(l, 0);
		}
	}

	/**
	*	\brief 函数Caller，对每个参数进行转换
	*/
	template<typename RetType>
	inline RetType FuncCaller(lua_State*l, int index, std::function<RetType()> f)
	{
		return f();
	}

	/**
	*	\brief 函数Caller，对每个参数进行转换
	*/
	template<typename RetType, typename T1, typename... T>
	inline RetType FuncCaller(lua_State*l, int index, std::function<RetType(T1, T...)> f)
	{
		using ArgType = std::remove_cv<std::remove_reference<T1>::type>::type;
		
		// DEBUG
		//std::cout << typeid(ArgType).name() << std::endl;

		ArgType arg = UnWrapperValue::UnWraper<ArgType>(l, index);
		return FuncCaller(l, index + 1, std::function<RetType(T...)>(
			[&](T...rest)->RetType {
			return f(std::forward<T1>(arg), std::forward<T>(rest)...);
		}
		));
	}

	template<typename RetType, typename... Args>
	static std::enable_if_t<std::is_void<RetType>::value, int>
		UnWraperLuaFunction(lua_State* l)
	{
		typedef RetType(*Func)(Args...);

		return LuaTools::ExceptionBoundary(l, [&] {
			Func func = static_cast<Func>(lua_touserdata(l, lua_upvalueindex(1)));
			int returnCount = LuaTools::CheckInt(l, lua_upvalueindex(2));
			if (func == nullptr)
			{
				Debug::Error("Faild to get function.");
				return 0;
			}
			FuncCaller(l, 1, std::function<RetType(Args...)>(func));

			// 返回值个数检查
			if (lua_gettop(l) - returnCount < sizeof...(Args))
			{
				Debug::Error("The returnValue count is error.");
				return 0;
			}
			return returnCount;
		});
	}

	template<typename RetType, typename... Args>
	static std::enable_if_t<!std::is_void<RetType>::value, int>
		UnWraperLuaFunction(lua_State* l)
	{
		typedef RetType(*Func)(Args...);

		return LuaTools::ExceptionBoundary(l, [&] {
			Func func = static_cast<Func>(lua_touserdata(l, lua_upvalueindex(1)));
			int returnCount = LuaTools::CheckInt(l, lua_upvalueindex(2));
			if (func == nullptr)
			{
				Debug::Error("Faild to get function.");
				return 0;
			}
			RetType result = FuncCaller(l, 1, std::function<RetType(Args...)>(func));
			WrapperValue::Wraper(l, result);
			// 返回值个数检查,?????????
			//if (lua_gettop(l) - returnCount <= sizeof...(Args))
			//{
			//	Debug::Error("The returnValue count is error.");
			//	return 0;
			//}
			return returnCount;
		});
	}

	template<typename T, typename RetType, typename... Args>
	static std::enable_if_t<std::is_void<RetType>::value, int>
		UnWraperLuaClassFunction(lua_State* l)
	{
		using MemberFunction = std::function<void(lua_State*l, LuaObject&)>;

		return LuaTools::ExceptionBoundary(l, [&] {
			// function
			MemberFunction* memberFunction = static_cast<MemberFunction*>(lua_touserdata(l, lua_upvalueindex(1)));
			if (memberFunction == nullptr)
			{
				Debug::Error("Faild to get function.");
				return 0;
			}

			// called class
			const std::string className = LuaTools::CheckString(l, lua_upvalueindex(2));
			LuaObjectPtr objPtr = LuaContext::CheckUserdata(l, 1, className);
			if (objPtr == nullptr)
			{
				Debug::Error("Call the function without self.");
				return 0;
			}

			(*memberFunction)(l, *objPtr);
			return 0;
		});
	}

	template<typename T, typename RetType, typename... Args>
	static std::enable_if_t<!std::is_void<RetType>::value, int>
		UnWraperLuaClassFunction(lua_State* l)
	{
		using MemberFunction = std::function<RetType(lua_State*l, LuaObject&)>;

		return LuaTools::ExceptionBoundary(l, [&] {
			// function
			MemberFunction* memberFunction = static_cast<MemberFunction*>(lua_touserdata(l, lua_upvalueindex(1)));
			if (memberFunction == nullptr)
			{
				Debug::Error("Faild to get function.");
				return 0;
			}

			// called class
			const std::string className = LuaTools::CheckString(l, lua_upvalueindex(2));
			LuaObjectPtr objPtr = LuaContext::CheckUserdata(l, 1, className);
			if (objPtr == nullptr)
			{
				Debug::Error("Call the function without self.");
				return 0;
			}

			RetType result = (*memberFunction)(l, *objPtr);
			WrapperValue::Wraper(l, result);
			return 1;
		});
	}


}
