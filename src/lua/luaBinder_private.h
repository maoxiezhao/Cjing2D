#pragma once

#include"lua\luaTools.h"
#include"lua\luaData.h"
#include"lua\luaContext.h"

#include<string>

/**
*	\brief ����������
*/
namespace Implemention
{
	/**
	*	\brief lua��C++���ݽ�����
	*/
	namespace UnWrapperValue
	{
		/** ���� */
		template<typename T>
		inline typename std::enable_if<std::is_integral<T>::value, T>::type
			UnWraper(lua_State*l, int index) { return static_cast<T>(LuaTools::CheckInt(l, index)); }
		/** ������ */
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
	}

	/**
	*	\brief C++��lua���ݽ�����
	*/
	namespace WrapperValue
	{
		/** ���� */
		template<typename T>
		inline std::enable_if_t<std::is_integral<T>::value>
			Wraper(lua_State*l, T value)
		{
			lua_pushinteger(l, value);
		}
		/** ������ */
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
	}

	/**
	*	\brief ����Caller����ÿ����������ת��
	*/
	template<typename RetType>
	inline RetType FuncCaller(lua_State*l, int index, std::function<RetType()> f)
	{
		return f();
	}

	/**
	*	\brief ����Caller����ÿ����������ת��
	*/
	template<typename RetType, typename T1, typename... T>
	inline RetType FuncCaller(lua_State*l, int index, std::function<RetType(T1, T...)> f)
	{
		using ArgType = std::remove_cv<std::remove_reference<T1>::type>::type;
		std::cout << typeid(ArgType).name() << std::endl;

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

			// ����ֵ�������
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
			// ����ֵ�������
			if (lua_gettop(l) - returnCount <= sizeof...(Args))
			{
				Debug::Error("The returnValue count is error.");
				return 0;
			}
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