#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"core\fileData.h"
#include"core\logger.h"

const string LuaContext::module_utils_name = "Utils";

int utils_api_log_info(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& msg = LuaTools::CheckString(l, 1);
		Logger::Info(msg);
		return 0;
	});
}

int utils_api_log_warn(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& msg = LuaTools::CheckString(l, 1);
		Logger::Warning(msg);
		return 0;
	});
}

int utils_api_log_err(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& msg = LuaTools::CheckString(l, 1);
		LuaTools::Error(l, msg);
		return 0;
	});
}

void LuaContext::RegisterUtils(lua_State* l)
{
	LuaContext::RegisterGlobalFunction(l, "util_log_info", utils_api_log_info);
	LuaContext::RegisterGlobalFunction(l, "util_log_warn", utils_api_log_warn);
	LuaContext::RegisterGlobalFunction(l, "util_log_err",  utils_api_log_err);

	//DefaultLuaBinder bindClass(l, module_utils_name);
}

