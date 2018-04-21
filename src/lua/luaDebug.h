#pragma once

#include"lua\luaTools.h"

#include<functional>

/**
*	\brief 基于控制台的本地Lua调试器
*/
class LuaDebug
{
public:
	static const int MAX_BREAK_POINT_NUMS = 120;
	static const int MAX_FILE_NUMS = 16;
	static const char* LUA_DEBUG_TAG;

	/** 断点描述 */
	struct BreakPoint {
		int line;
		bool availble;

		BreakPoint() :line(0), availble(false) {}
	};

	/** Debug描述 */
	struct DebugInfo {
		int depth;
		int step;
		bool first;
		int breakPointNum;
		BreakPoint breakPoints[MAX_BREAK_POINT_NUMS];
	};

	/** Debug handle */
	using DebugHandleFunc = std::function<bool(lua_State*l, DebugInfo& info, lua_Debug* ar, std::string inputStr)>;
	struct DebugHandle {
		std::string commandName;
		std::string commandTip;
		DebugHandleFunc handleFunc;

		DebugHandle(
			const std::string n,
			const std::string t,
			DebugHandleFunc func) :
			commandName(n),
			commandTip(t),
			handleFunc(func) {}
	};

	LuaDebug(lua_State* l);
	~LuaDebug();

	void Initialize();
	void Uninitialize();
	bool IsInitialize()const;

	static void DebugBreak(lua_State* l);
	static DebugInfo& CheckDebugInfo(lua_State* l);
	static void SingleStep(lua_State*l, DebugInfo& info, int step);
	static void EnableLineHook(lua_State*l, bool enable);
	static void OnEvent(int bp, DebugInfo& info, lua_State*l, lua_Debug* ar);

	/** hook function */
	static void HookCallback(lua_State *l, lua_Debug *ar);

	/** debug api funciton */
	using FunctionExportToLua = int(lua_State* l);
	static FunctionExportToLua
		debug_api_break;

	static std::map<std::string, DebugHandle> debugCommands;
	static std::map<std::string, std::vector<string> > debugFiles;
private:
	lua_State* l;
	DebugInfo mInfo;
	bool mIsInitialized;
};