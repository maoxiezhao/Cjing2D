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

int utils_api_to_degree(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		float radians = LuaTools::CheckNumber(l, 1);
		lua_pushnumber(l, Geometry::Degree(radians));
		return 1;
	});
}


int utils_api_to_radians(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		float degree = LuaTools::CheckNumber(l, 1);
		lua_pushnumber(l, Geometry::Radians(degree));
		return 1;
	});
}

void LuaContext::RegisterUtils(lua_State* l)
{
	LuaContext::RegisterGlobalFunction(l, "util_log_info", utils_api_log_info);
	LuaContext::RegisterGlobalFunction(l, "util_log_warn", utils_api_log_warn);
	LuaContext::RegisterGlobalFunction(l, "util_log_err",  utils_api_log_err);
	LuaContext::RegisterGlobalFunction(l, "util_to_degree", utils_api_to_degree);
	LuaContext::RegisterGlobalFunction(l, "util_to_radians", utils_api_to_radians);
	//DefaultLuaBinder bindClass(l, module_utils_name);
}

