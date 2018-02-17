#pragma once
#include<string>

/**
*	\brief 初始化lua环境的字符序列
*/
const std::string luaEnvInitScript =
// 初始化lua脚本环境
"SystemEnvTables = {__G__ = _ENV}\n"
"SystemEnvTables.Enum = {}\n"
"SystemEnvTables.Const = SystemEnvTables.Enum\n"
"SystemEnvTables.CApi = {}\n"
"SystemEnvTables.Modules = {}\n"
"SystemEnvTables.Imports = {}\n"
"\n"
"SystemExports = {}\n"
"GlobalExports = setmetatable({}, {__index = _ENV, __newindex=function(t,k,v)rawset(_ENV, k, v);  end})\n"
"setmetatable(_ENV, {__index = SystemEnvTables.CApi })\n"
// 初始化lua基本方法
"SystemLoadScript = function(script_cxt, name, env) \n"
"assert(env, 'env must exist.')\n"
"return load(script_cxt, name, 'bt', env)\n"
"end\n"
"SystemDoString = function(script, name, env, ...)\n"
"local f,err = SystemLoadScript(script, name, env or _ENV)\n"
"if f==nil then error(err) end\n"
"return f(...)\n"
"end\n"
"\n"
// 初始化lua 执行文件接口
"SystemDoFile = function(script, env, ...)\n"
"if not FileData.Exists(script) then error('File '..script..' not exists.'); return; end\n"
"local buf = FileData.Read(script)"
"local f,err = load(buf, script, 'bt', env or _ENV) \n"
"if f==nil then error(err) end\n"
"return f(...)"
"end\n"
"\n"
"local traceBack = debug.traceback\n"
"local _logger = nil\n"
"SystemLogErrHandle = function(err)err = _traceback(err,2);return err; end\n"
"SystemTraceDoFile = function(script, env, ...)\n"
"local ok, res = xpcall(SystemDoFile, SystemLogErrHandle, script, env or _ENV, ...)\n"
"if ok then return res end\n"
"end\n"
// 初始化lua modules环境
"local modules = SystemEnvTables.Modules\n"
"local imports = SystemEnvTables.Imports\n"
"SystemModule = function(name)\n"
"  local m = modules[name]\n"
"  if not m then\n"
"   m = setmetatable({}, {__index=_ENV})\n"
"   modules[name] = m\n"
"  end\n"
"  return m\n"
"end\n"
"\n"
;
