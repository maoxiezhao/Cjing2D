#pragma once

#include"lua\luaTools.h"
#include"lua\luaData.h"
#include"lua\luaContext.h"
#include"lua\luaBinder_private.h"

#include<functional>
#include<vector>

/**
*	\brief luaBinderModule
*
*	luaBinderModule设计用来分离各个模块的api和luaContext的关系
*	减少绑定lua的繁琐步骤。
*
*	luaBinder作为一个bindModule用来代替luaContext中
*	的bind过程，但luaContext作为上下文环境，依然存在,
*	用来维护添加的数据
*/

/**
*	\brief 自动执行绑定过程
*/
#define LUA_FUNCTION_AUTO_BINDER(LuaName, FuncName) \
	class AutoBinder##LuaName{				\
		public:AutoBinder##LuaName(){		\
			LuaBinder::GetInstance().PushRegisterFunction(FuncName); }	\
		static AutoBinder##LuaName instance;};	\
	 AutoBinder##LuaName  AutoBinder##LuaName::instance;

/**
*	\brief luaBinder负责管理所有的绑定过程
*	同时保存每个模块所对应的luaState
*/
class LuaBinder
{
public:
	using RegisterFunction = std::function<void(lua_State* l)>;

	/**
	*	\brief 执行注册函数，该过程一般由luaContext初始化时调用
	*/
	void Initialize(lua_State* l)
	{
		for (auto func : mRegisterFunctions)
		{
			func(l);
		}
	}
	/**
	*	\brief 添加注册函数，该过程一般由宏自动完成
	*/
	void PushRegisterFunction(std::function<void(lua_State* l)> func)
	{
		mRegisterFunctions.push_back(func);
	}
	/**
	*	\brief 保存指定模块的luaState
	*/
	void PushModuleLuaState(const std::string& moduleName, lua_State*l)
	{
		mModuleStateMapping[moduleName] = l;
	}
	/**
	*	\brief 过去指定模块的luaState
	*/
	lua_State* GetModuleLuaState(const std::string& moduleName)
	{
		return mModuleStateMapping[moduleName];
	}

	static LuaBinder& GetInstance()
	{
		static LuaBinder luaBinder;
		return luaBinder;
	}

private:
	LuaBinder() {}
	std::vector<RegisterFunction> mRegisterFunctions;
	std::map<std::string, lua_State*> mModuleStateMapping;
};

/**
*	\brief C++绑定lua类
*
*	该类实现所有的绑定过程
*/
template<typename T>
class LuaBindClass
{
public:
	using FunctionExportToLua = int(lua_State* l);
	LuaBindClass(lua_State* l, const std::string& name, const std::string& baseName = "");

	/** const member */

	/** Meta function */
	void AddMetaFunction(const std::string& key, FunctionExportToLua func);

	/** Adding Global Function */
	template<typename RetType, typename ...Args>
	void AddFunction(const std::string& funcName, RetType(*f)(Args...), int resultCount = 1);
	template<typename... Args>
	void AddFunction(const std::string& funcName, void(*f)(Args...), int resultCount = 0);

	/** Adding Method */
	template<typename RetType, typename... Args>
	void AddMethod(const std::string& methodName, RetType(T::*f)(Args...));
	template<typename... Args>
	void AddMethod(const std::string& methodName, void(T::*f)(Args...));
	template<typename RetType, typename... Args>
	void AddMethod(const std::string& methodName, RetType(T::*f)(Args...)const);
	template<typename... Args>
	void AddMethod(const std::string& methodName, void(T::*f)(Args...)const);

private:
	std::string mName;
	lua_State* l;
};

class LuaBinderSpacingClass {};	// 用于设置默认绑定类的空白类
using DefaultLuaBinder = LuaBindClass<LuaBinderSpacingClass>; // 默认的绑定类

template<typename T>
inline LuaBindClass<T>::LuaBindClass(lua_State* l, const std::string& name, const std::string& baseName) :
	mName(name),
	l(l)
{
	if (baseName.size() == 0)
	{
		LuaContext::RegisterType(l, name, nullptr, nullptr, nullptr);
	}
	else
	{
		LuaContext::RegisterType(l, name, baseName, nullptr, nullptr, nullptr);
	}
	LuaBinder::GetInstance().PushModuleLuaState(mName, l);
}

template<typename T>
inline void LuaBindClass<T>::AddMetaFunction(const std::string& key, FunctionExportToLua func)
{
	LuaContext::RegisterMetaFunction(l, mName, key, func);
}

template<typename T>
template<typename RetType, typename ...Args>
inline void LuaBindClass<T>::AddFunction(const std::string & funcName, RetType(*f)(Args...), int resultCount)
{
	LuaContext::PushRef(l, LuaContext::mSystemModulesRef);
	lua_getfield(l, -1, mName.c_str());
	if (lua_isnil(l, -1))
	{
		lua_pop(l, 1);
		lua_newtable(l);
		// cjing module
		lua_pushvalue(l, -1);
		// cjing module module
		lua_setfield(l, -3, mName.c_str());
	}
	// cjing module
	lua_pushstring(l, funcName.c_str());
	// cjing module funcName
	lua_pushlightuserdata(l, reinterpret_cast<void*>(f));
	lua_pushinteger(l, resultCount);
	lua_pushcclosure(l, Implemention::UnWraperLuaFunction<RetType, Args...>, 2);
	// cjing module funcName func
	lua_settable(l, -3);
	// cjing module
	lua_pop(l, 2);
}

template<typename T>
template<typename ...Args>
inline void LuaBindClass<T>::AddFunction(const std::string & funcName, void(*f)(Args...), int resultCount)
{
	LuaContext::PushRef(l, LuaContext::mSystemModulesRef);
	lua_getfield(l, -1, mName.c_str());
	if (lua_isnil(l, -1))
	{
		lua_pop(l, 1);
		lua_newtable(l);
		// cjing module
		lua_pushvalue(l, -1);
		// cjing module module
		lua_setfield(l, -3, mName.c_str());
	}
	// cjing module
	lua_pushstring(l, funcName.c_str());
	// cjing module funcName
	lua_pushlightuserdata(l, reinterpret_cast<void*>(f));
	lua_pushinteger(l, resultCount);
	lua_pushcclosure(l, Implemention::UnWraperLuaFunction<void, Args...>, 2);
	// cjing module funcName func
	lua_settable(l, -3);
	// cjing module
	lua_pop(l, 2);
}


template<typename T>
template<typename RetType, typename ...Args>
inline void LuaBindClass<T>::AddMethod(const std::string & methodName, RetType(T::* f)(Args...))
{
	luaL_getmetatable(l, mName.c_str());
	// metatable
	lua_pushstring(l, methodName.c_str());
	// metatable key
	using MemberFunction = std::function<RetType(lua_State*l, LuaObject&)>;
	MemberFunction* func = LuaTools::LuaBindingUserdata<MemberFunction>::Create(l, "_member_function");
	// metatable key func
	*func = [f](lua_State*l, LuaObject& obj)->RetType {
		T& realObj = dynamic_cast<T&>(obj);
		return Implemention::FuncCaller(l, 2, std::function<RetType(Args...)>(
			[&realObj, f](Args... args) {
			return (realObj.*f)(std::forward<Args>(args)...);
		}));
	};
	lua_pushstring(l, mName.c_str());
	// metatable key func str
	lua_pushcclosure(l, Implemention::UnWraperLuaClassFunction<T, RetType, Args...>, 2);
	// metatable key closure
	lua_settable(l, -3);
	lua_pop(l, 1);
}

template<typename T>
template<typename ...Args>
inline void LuaBindClass<T>::AddMethod(const std::string & methodName, void(T::* f)(Args...))
{
	luaL_getmetatable(l, mName.c_str());
	// metatable
	lua_pushstring(l, methodName.c_str());
	// metatable key
	using MemberFunction = std::function<void(lua_State*l, LuaObject&)>;
	MemberFunction* func = LuaTools::LuaBindingUserdata<MemberFunction>::Create(l, "_member_function");
	// metatable key func
	*func = [f](lua_State*l, LuaObject& obj) {
		T& realObj = dynamic_cast<T&>(obj);
		Implemention::FuncCaller(l, 2, std::function<void(Args...)>(
			[&realObj, f](Args... args) {
			(realObj.*f)(std::forward<Args>(args)...);
		}));
	};
	lua_pushstring(l, mName.c_str());
	lua_pushcclosure(l, Implemention::UnWraperLuaClassFunction<T, void, Args...>, 2);
	lua_settable(l, -3);
	lua_pop(l, 2);
}


template<typename T>
template<typename RetType, typename ...Args>
inline void LuaBindClass<T>::AddMethod(const std::string & methodName, RetType(T::* f)(Args...)const)
{
	luaL_getmetatable(l, mName.c_str());
	// metatable
	lua_pushstring(l, methodName.c_str());
	// metatable key
	using MemberFunction = std::function<RetType(lua_State*l, LuaObject&)>;
	MemberFunction* func = LuaTools::LuaBindingUserdata<MemberFunction>::Create(l, "_member_function");
	// metatable key func
	*func = [f](lua_State*l, LuaObject& obj)->RetType {
		T& realObj = dynamic_cast<T&>(obj);
		return Implemention::FuncCaller(l, 2, std::function<RetType(Args...)>(
			[&realObj, f](Args... args) {
			return (realObj.*f)(std::forward<Args>(args)...);
		}));
	};
	lua_pushstring(l, mName.c_str());
	lua_pushcclosure(l, Implemention::UnWraperLuaClassFunction<T, RetType, Args...>, 2);
	lua_settable(l, -3);
	lua_pop(l, 2);
}

template<typename T>
template<typename ...Args>
inline void LuaBindClass<T>::AddMethod(const std::string & methodName, void(T::* f)(Args...)const)
{
	luaL_getmetatable(l, mName.c_str());
	// metatable
	lua_pushstring(l, methodName.c_str());
	// metatable key
	using MemberFunction = std::function<void(lua_State*l, LuaObject&)>;
	MemberFunction* func = LuaTools::LuaBindingUserdata<MemberFunction>::Create(l, "_member_function");
	// metatable key func
	*func = [f](lua_State*l, LuaObject& obj) {
		T& realObj = dynamic_cast<T&>(obj);
		Implemention::FuncCaller(l, 2, std::function<void(Args...)>(
			[&realObj, f](Args... args) {
			(realObj.*f)(std::forward<Args>(args)...);
		}));
	};
	lua_pushstring(l, mName.c_str());
	lua_pushcclosure(l, Implemention::UnWraperLuaClassFunction<T, void, Args...>, 2);
	lua_settable(l, -3);
	lua_pop(l, 2);
}

namespace LuaSystemBind {
/**
*	\brief 用于定义全局函数，该函数解析传入的参数和返回值
*	调用luaContext的registerFunction将函数绑定到全局CAPI中
*/
template<typename RetType, typename ...Args>
void RegisterSystemFunction(lua_State*l,  std::string& funcName, RetType(*f)(Args...), int resultCount = 1)
{
	lua_pushlightuserdata(l, reinterpret_cast<void*>(f));
	lua_pushinteger(l, resultCount);
	lua_pushcclosure(l, Implemention::UnWraperLuaFunction<RetType, Args...>, 2);
	// func	
	LuaContext::RegisterFunction(l, funcName, lua_tocfunction(l, -1));
}

template<typename... Args>
void RegisterSystemFunction(const std::string& funcName, void(*f)(Args...))
{
	lua_pushlightuserdata(l, reinterpret_cast<void*>(f));
	lua_pushinteger(l, resultCount);
	lua_pushcclosure(l, Implemention::UnWraperLuaFunction<void, Args...>, 2);
	// func	
	LuaContext::RegisterFunction(l, funcName, lua_tocfunction(l, -1));
}
	

}