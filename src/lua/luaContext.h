#ifndef _LUACONTEXT_H_
#define _LUACONTEXT_H_

#include"common\common.h"
#include"lua\luaTools.h"
#include"lua\luaRef.h"
#include"game\timer.h"

#include<map>
#include<list>
#include<thirdparty\lua.hpp>

/**
 *	\brief C++和lua的接口，提供与用于lua使用的C++ API
 *
 *  其中的提供的scripts包括main,sprite,map,entity等
 *  其中所有的script运行在同一个luaContext
 */
class LuaContext
{
public:
	LuaContext();
	~LuaContext();

	// system
	void Initialize();
	void Update();
	void Exit();

	// script
	static LuaContext& GetLuaContext(lua_State* l);
	bool DoFileIfExists(lua_State*l,const string& name);
	bool LoadFile(lua_State*l, const string& name);
	bool FindMethod(const string& name);
	bool FindMethod(const string& name, int index);
	void RegisterFunction(const string& moduleName, const luaL_Reg* functions);

	// process
	void OnStart();
	void OnUpdate();
	void OnFinish();
	
	// modules
	void RegisterModules();
	void RegisterMainModule();
	void RegisterTimeModule();

	// binding function
	using FunctionExportToLua = int(lua_State* l);
	static FunctionExportToLua
		// error
		api_panic,
		// main
		main_api_hello,
		
		// time
		time_api_start;

	// main api	-- test 
	void PushMain(lua_State*l);
	void OnMainStart();
	void OnMainUpdate();
	void OnMainFinish();

	// time api
	struct TimerData
	{
		LuaRef callBackRef;
		const void* l;
	};
	void AddTimer(const TimerPtr& timer,int constext,const LuaRef& callback);
	void CallTimerRef(const TimerPtr& timer);
	void UpdateTimers();
	void DestoryTimers();

	// modules name
	static const string module_prefix_name;
	static const string module_main_name;
	static const string module_time_name;

private:
	lua_State* l;
	static std::map<lua_State*, LuaContext*>mLuaContexts;/* 用于在lua APi中通过lua_state获取
														    luaContext */

	std::map<TimerPtr, TimerData>mTimers;				 /* 存储了定时器，每个定时器映射的对
														    应的callback*/
	std::list<TimerPtr>mTimersToRemove;

};

#endif