#include "lua\luaDebug.h"
#include "lua\luaContext.h"
#include "lua\luaTools.h"
#include "core\fileData.h"
#include "core\debug.h"
#include "core\logger.h"
#include "utils\commonUtils.h"

#include <iostream>

const char* LuaDebug::LUA_DEBUG_TAG = "__lua_debug__";

namespace {
	std::vector<std::string> GetLuaSplitByLines(const std::string& fileName)
	{
		std::vector<std::string> fileLines;
		std::string fileData = FileData::ReadFile(fileName);
		if (fileData.size() > 0)
		{
			fileLines = util::SplitString(fileData, "\n");
		}
		return fileLines;
	}

	/** 输出当前行 */
	void PrintCurrentLine(LuaDebug::DebugInfo& info, lua_Debug* ar)
	{
		std::string curLineStr = "";
		int curLine = ar->currentline;

		const std::string& fileName = ar->source;
		auto it = LuaDebug::debugFiles.find(fileName);
		if (it == LuaDebug::debugFiles.end())
		{
			auto lines = GetLuaSplitByLines(fileName);
			curLineStr = lines[curLine - 1];
			LuaDebug::debugFiles[fileName] = lines;
		}
		else
		{
			auto lines = it->second;
			curLineStr = lines[curLine];
		}

		curLine += info.first ? 0 : 1;
		Logger::Debug("[CurBreak]"  + fileName + " lines " + std::to_string(curLine) +
			":" + curLineStr);
	}

	void PrintLuaDebugHelp()
	{
		Logger::Debug("Input debugging command:");
	} 

	/** 输出所有命令信息 */
	bool PrintAllCommands(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		for (const auto& kvp : LuaDebug::debugCommands)
		{
			const auto& handler = kvp.second;
			Logger::Debug("Command Name:'" + handler.commandName + 
				"' Command Desc:" + handler.commandTip);
		}
		return true;
	}

	/** 继续执行lua脚本 */
	bool ContinueExcuteLua(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		info.step = 0;
		info.depth = -1;
	
		LuaDebug::EnableLineHook(l, false);
		return false;
	}

	/** 执行一段lua字符串 */
	bool DoLuaString(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		if (luaL_loadstring(l, inputStr.c_str()) == 0)
		{
			LuaTools::CallFunction(l, 0, 0, "Load Lua String.");
		}
		return true;
	}

	/** 打印当前Lua栈 */
	bool PrintLuaStack(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		lua_Debug db;
		std::cout << "[TrackBack]" << std::endl;
		for (int i = 0; lua_getstack(l, i, &db) == 1; i++)
		{
			if (lua_getinfo(l, "lnSu", &db))
			{
				std::string name = db.name != nullptr ? db.name : "[C]";
				std::ostringstream oss;
				oss << "[" << i + 1 << "]:";
				oss << db.source << " line:" << db.currentline << ": ";
				oss << std::string(db.what) << ":" << name;
				std::cout << oss.str() << std::endl;
			}
		}
		return true;
	}

	/** 打印指定变量的值 */
	bool PrintVarValue(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		if (inputStr.empty())
		{
			Logger::Debug("Input valid varname.");
			return true;
		}
		// 先从local中找
		const char* curName = nullptr;
		for (int i = 1; (curName = lua_getlocal(l, ar, i)) != nullptr; i++)
		{
			if (inputStr == std::string(curName))
			{
				LuaTools::PrintInIndex(l, -1);
				lua_pop(l, 1);
				return true;
			}
			lua_pop(l, 1);
		}

		// 上值處理
		static const std::string luaString =
			"local func, name = ...\n"
			"do\n"
			"local i = 1\n"
			"while true do\n"
			"local n, v = debug.getupvalue(func, i)\n"
			"if not n then break end\n"
			"if n == '_ENV' and type(v) == 'table' then -- 仅左一层_ENV检测\n"
			"for key, value in pairs(v) do\n"
			"if key == name then\n"
			"print(value)\n"
			"return true\n"
			"end\n"
			"end\n"
			"elseif n == name then\n"
			"print(v)\n"
			"return true\n"
			"end\n"
			"i = i + 1\n"
			"end\n"
			"return false\n"
			"end\n";
		if (luaL_loadstring(l, luaString.c_str()) == 0)
		{
			lua_getinfo(l, "lnSuf", ar);
			lua_pushstring(l, inputStr.c_str());
			LuaTools::CallFunction(l, 2, 1, "Load Lua String.");
			if (LuaTools::CheckBoolean(l, -1))
				return true;
		}

		// 当前全局环境,
		lua_getglobal(l, inputStr.c_str());
		if (!lua_isnil(l, -1))
		{
			LuaTools::PrintInIndex(l, -1);
			lua_pop(l, 1);
			return true;
		}
		lua_pop(l, 1);	
		Logger::Debug("nil");
		return true;
	}

	/** 单步执行 */
	bool StepDo(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		if (info.step != 1)
			LuaDebug::SingleStep(l, info, 1);
		PrintCurrentLine(info, ar);

		info.depth = -1;
		return false;
	}

	/** 步进（不进入函数） */
	bool NextDo(lua_State*l, LuaDebug::DebugInfo& info, lua_Debug* ar, std::string inputStr)
	{
		if (info.step != 1)
			LuaDebug::SingleStep(l, info, 1);
		PrintCurrentLine(info, ar);

		info.depth = 0;
		return false;
	}
}

std::map<std::string, LuaDebug::DebugHandle> LuaDebug::debugCommands = {
	{"\\h",{"\\h", "Print all debug commands.", PrintAllCommands }},
	{"\\continue",{ "\\continue", "Continue excute lua.", ContinueExcuteLua } },
	{"\\do", {"\\do", "Do a lua string.", DoLuaString }},
	{"\\printStack", {"\\printStack", "Print lua stack.", PrintLuaStack }},
	{ "\\print",{ "\\print", "Print varname.", PrintVarValue } },
	{ "\\step",{ "\\step", "Single step.", StepDo } },
	{ "\\next",{ "\\next", "Single next.", NextDo } },
};

std::map<std::string, std::vector<string> > LuaDebug::debugFiles = {};


LuaDebug::LuaDebug(lua_State * l):
	l(l),
	mIsInitialized(false)
{
	Initialize();
}

LuaDebug::~LuaDebug()
{
}

void LuaDebug::Initialize()
{
	Debug::CheckAssertion(l != nullptr,
		"Lua Debugger initialize failed without lua state.");

	// Init debug info 
	mInfo.step = 0;
	mInfo.depth = 0;
	mInfo.first = true;
	mInfo.breakPointNum = 0;

	lua_pushstring(l, LUA_DEBUG_TAG);
	lua_pushlightuserdata(l, &mInfo);
	lua_settable(l, LUA_REGISTRYINDEX);

	LuaContext::RegisterGlobalFunction(l, "debug_break_point", debug_api_break);

	mIsInitialized = true;
}

void LuaDebug::Uninitialize()
{
	if (IsInitialize())
	{
		debugCommands.clear();
		debugFiles.clear();

		lua_pushstring(l, LUA_DEBUG_TAG);
		lua_pushlightuserdata(l, nullptr);
		lua_settable(l, LUA_REGISTRYINDEX);
	}
}

bool LuaDebug::IsInitialize() const
{
	return mIsInitialized;
}

void LuaDebug::DebugBreak(lua_State * l)
{
	auto& debugInfo = CheckDebugInfo(l);
	if (debugInfo.step == 0)
		SingleStep(l, debugInfo, 1);

	debugInfo.step = 1;
	debugInfo.first = true;
	debugInfo.depth = -1;
}

LuaDebug::DebugInfo & LuaDebug::CheckDebugInfo(lua_State * l)
{
	lua_pushstring(l, LUA_DEBUG_TAG);
	lua_gettable(l, LUA_REGISTRYINDEX);
	DebugInfo* debugInfo = static_cast<DebugInfo*>(lua_touserdata(l, -1));
	if (debugInfo == nullptr)
		Debug::Error("Excepted got luaDebugInfo,but got nil.");
	return *debugInfo;
}

void LuaDebug::SingleStep(lua_State * l, DebugInfo & info, int step)
{
	if (step > 0)
		EnableLineHook(l, true);
	info.step = step;
}

void LuaDebug::EnableLineHook(lua_State * l, bool enable)
{
	int mask = lua_gethookmask(l);
	mask = enable ? mask | LUA_MASKLINE : mask & ~LUA_MASKLINE;
	lua_sethook(l, HookCallback, mask, 0);
}

/**
*	\brief 断点事件处理
*/
void LuaDebug::OnEvent(int bp, DebugInfo & info, lua_State * l, lua_Debug * ar)
{
	int callDepth = LuaTools::GetCallDepth(l);
	if (bp < 0 && info.depth != -1 && callDepth > info.depth)
		return;
	info.depth = callDepth;

	PrintLuaDebugHelp();
	char str[128] = {};
	while (std::cin.getline(str, 128))
	{
		std::string inputStr(str);
		if(inputStr.empty())
			continue;

		// 根据空格分隔command和额外的输入
		std::string command = inputStr;
		std::string extraStr = "";
		int spaceIndex = inputStr.find(" ");
		if (spaceIndex != std::string::npos)
		{
			command = inputStr.substr(0, spaceIndex);
			extraStr = inputStr.substr(spaceIndex + 1);
		}

		// handle command
		bool ret = true;
		auto findItr = debugCommands.find(command);
		if (findItr != debugCommands.end())
		{
			const auto& handler = findItr->second;
			ret = handler.handleFunc(l, info, ar, extraStr);
		}
		
		if (!ret)
			break;
	}
}

/**
*	\brief Lua 钩子回调函数
*/
void LuaDebug::HookCallback(lua_State * l, lua_Debug * ar)
{
	auto& debugInfo = CheckDebugInfo(l);
	if (!lua_getstack(l, 0, ar)) {
		Debug::Warning("Failed to get lua stack.");
		return;
	}
	/*//////////////////////////////////////////////////////////////////////////////
	'n': 填充 name 及 namewhat 域；
	'S': 填充 source ， short_src ， linedefined ， lastlinedefined ，以及 what 域；
	'l': 填充 currentline 域；
	't': 填充 istailcall 域；
	'u': 填充 nups， nparams，及 isvararg 域；
	'f': 把正在运行中指定层次处函数压栈；
	'L': 将一张表压栈，这张表中的整数索引用于描述函数中哪些行是有效行。 
		（有效行指有实际代码的行，即你可以置入断点的行。 无效行包括空行和只有注释的行。）
	*////////////////////////////////////////////////////////////////////////////////
	if (lua_getinfo(l, "lnSu", ar))
	{
		if (debugInfo.step > 0)
		{
			if (debugInfo.first) {
				Logger::Debug("Break in " + std::string(ar->source) + "[" + std::to_string(ar->currentline) + "]");
				PrintCurrentLine(debugInfo, ar);
				debugInfo.first = false;
			}
		}
		OnEvent(-1, debugInfo, l, ar);
	}
	else
	{
		Debug::Warning("Failed to get lua info.");
	}
}

int LuaDebug::debug_api_break(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaDebug::DebugBreak(l);
		return 0;
	});
}