#include"luaContext.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"core\logger.h"
#include"game\app.h"
#include"game\enumInfo.h"
#include<sstream>

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
"Global_Exports = setmetatable({}, {__index = _ENV, __newindex=function(t,k,v)rawset(_ENV, k, v)  end})\n"
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
"if cjing.FileData.Exists(script) then error('File '..script..' not exists.'); return; end\n"
"local buf = cjing.FileData.Read(script)"
"local f,err = load(buf, script, env or _ENV) \n"
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
" local modules = SystemEnvTables.Modules\n"
" local imports = SystemEnvTables.Imports\n"
" SystemModule = function(name)\n"
"  local m = modules[name]\n"
"  if not m then\n"
"   m = setmetatable({}, {__index=_ENV})\n"
"   modules[name] = m\n"
"  end\n"
"  return m\n"
" end\n"
"\n"
;




std::map<lua_State*, LuaContext*> LuaContext::mLuaContexts;
const string LuaContext::module_name = "cjing";
int LuaContext::mSystemCApiRef = LUA_REFNIL;

LuaContext::LuaContext(App& app):
	mApp(app)
{
}

LuaContext::~LuaContext()
{
	this->Exit();
}

/**
*	\brief 初始化lua
*
*	包括初始化lua_state，注册各个模块，创建userdata table
*/
void LuaContext::Initialize()
{
	l = luaL_newstate();
	lua_atpanic(l, api_panic);
	luaL_openlibs(l);
	mLuaContexts[l] = this;

	// 初始化userdata环境
	InitUserdataEnv(l);

	// 初始化lua文件系统的注册
	RegisterFileData(l);

	// 自定义lua require 加载器，用于解析打包的资源文件
	if (!DoLuaString(l, luaEnvInitScript))
	{
		Debug::Error("Failed to load lua initialzed scripts.");
		return;
	}

	lua_getglobal(l, "SystemEnvTables");
	if (lua_isnil(l, -1))
	{
		Debug::Error("Faile to load lua iitialized scripts");
		return;
	}
	lua_getfield(l, -1, "CApi");
	Debug::CheckAssertion(!lua_isnil(l, 1), "Lua env initialized failed.");
	mSystemCApiRef = luaL_ref(l, LUA_REGISTRYINDEX);

	lua_pop(l, 1);	// pop SystemEnvTables


	// 加载全局函数, 准备废弃，全局函数在luaEnvScript中定义
	DoFileIfExists(l, "script/libFunction");

	// 注册模块
	RegisterModules();
	
	// 加载main脚本,并从start出开始执行
	Debug::CheckAssertion(lua_gettop(l) == 0, "The lua stack is not empty.");
	DoFileIfExists(l,"main");
	Debug::CheckAssertion(lua_gettop(l) == 0, "The lua stack is not empty.");

	OnMainStart();
}

/**
*	\brief 刷新Lua
*
*	包括刷新timers,menu,drawable,movements
*/
void LuaContext::Update()
{
	UpdateTimers();
	UpdateDrawables();
	UpdateAsyncLoaders();
	UpdateMenus();

	OnMainUpdate();

	if (lua_gettop(l) != 0)
	{
		PrintLuaStack(l);
		// 在异步加载的过程中可能会产生
		// nil,这里是临时解决办法
		lua_settop(l, 0);
	}


	Debug::CheckAssertion(lua_gettop(l) == 0,
		"There are something in lua stack after update");
}


/**
*	\brief 关闭lua
*/
void LuaContext::Exit()
{
	if (l != nullptr)
	{
		DestoryTimers();
		DestoryMenus();
		DestoryDrawables();
		DestoryAsyncLoaders();
		CloseUserdatas();

		lua_close(l);
		mLuaContexts.erase(l);
		l = nullptr;
	}
}

App& LuaContext::GetApp() const
{
	return mApp;
}

/**
*	\brief 响应输入事件
*
*   调用OnMainInput函数，由main去处理事件
*/
bool LuaContext::NotifyInput(const InputEvent & event)
{
	Debug::CheckAssertion(lua_gettop(l) == 0, "Stack must empty before NotifyInput.");
	bool handle = OnMainInput(event);
	Debug::CheckAssertion(lua_gettop(l) == 0, "Stack must empty after NotifyInput.");
	return handle;
}

bool LuaContext::DoLuaString(lua_State*l, const string& luaString)
{
	if (luaL_loadstring(l, luaString.c_str()) == 0)
	{
		LuaTools::CallFunction(l, 0, 0, "Load Lua String.");
		return true;
	}
	return false;
}

bool LuaContext::DoFileIfExists(lua_State* l, const string& name)
{
	if (LoadFile(l, name))
	{
		// if exists,call function
		LuaTools::CallFunction(l,0,0,name);
		return true;
	}
	return false;
}


/**
*	\brief 加载一个lua文件，如果文件存在则置于栈顶
*/
bool LuaContext::LoadFile(lua_State* l, const string& name)
{
	string fileName(name);
	// 如果不存在，在末尾加上Lua后再尝试
	if(!FileData::IsFileExists(fileName))
		fileName += ".lua";

	// 如果依旧不存在则返回
	if (!FileData::IsFileExists(fileName))
		return false;

	const string buffer = FileData::ReadFile(fileName);
	int result = luaL_loadbuffer(l, buffer.data(), buffer.size(), fileName.c_str());
	if (result != 0)
	{
		lua_pop(l, 1);
		// error
		return false;
	}
	return true;
}

bool LuaContext::FindMethod(const string& name)
{
	return FindMethod(name,-1);
}

/**
*	\brief 查找指定索引处的方法，如果存在则将方法置于栈顶
*
*	\return如果方法不存在，返回false
*/
bool LuaContext::FindMethod(const string & name, int index)
{
	int positiveIndex = LuaTools::GetPositiveIndex(l,index);	// 获得绝对索引
	
	if (lua_isnil(l, positiveIndex))
		return false;
							// -- object ---
	lua_getfield(l, positiveIndex, name.c_str());
							// -- object -- function/nil
	if (!lua_isfunction(l, -1))
	{
		lua_pop(l, 1);
		return false;
	}
	lua_pushvalue(l, positiveIndex);// object要作为参数
							// -- object -- function object
	return true;
}

void LuaContext::InitUserdataEnv(lua_State* l)
{
	// 创建userdata 表,设置模式为弱表
	lua_newtable(l);
	// userdata
	lua_newtable(l);
	// userdata meta
	lua_pushstring(l, "v");
	// userdata meta v
	lua_setfield(l, -2, "__mode");
	// userdata meta
	lua_setmetatable(l, -2);
	// userdata
	lua_setfield(l, LUA_REGISTRYINDEX, "all_userdata");

	// 创建userdata_table
	lua_newtable(l);
	lua_setfield(l, LUA_REGISTRYINDEX, "userdata_tables");

	lua_newtable(l);
	lua_setglobal(l, module_name.c_str());
}

/**
*	\brief userdata的终接器
*/
int LuaContext::userdata_meta_gc(lua_State*l)
{
	LuaObjectPtr* luaObject = static_cast<LuaObjectPtr*>(lua_touserdata(l, 1));
	if (luaObject != nullptr)		// 判空的存在内存泄露的可能
		luaObject->~shared_ptr<LuaObject>();
	return 0;
}

/**
*	\biref lua newindex元方法处理存储特殊数据
*
*	xxx = yyyy,那么newindex方法会将3个参数以userdata,xxx,yyy压栈,
*	如果userdata_tables里已经存在tables，则直接存取，不然
*/
int LuaContext::userdata_meta_newindex(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaTools::CheckType(l, 1, LUA_TUSERDATA);
		LuaTools::CheckAny(l, 2);
		//LuaTools::CheckAny(l, 3);

		LuaContext& luaContext = GetLuaContext(l);
		//luaContext.PrintLuaStack(l);
		const LuaObjectPtr& userdata = *static_cast<LuaObjectPtr*>(lua_touserdata(l, 1));
		Debug::CheckAssertion(userdata->IsKnowToLua(), "Invalid userdata without binding lua.");

		lua_getfield(l, LUA_REGISTRYINDEX, "userdata_tables");
		Debug::CheckAssertion(!lua_isnil(l, -1), "userdatas tables is not init.");
								// userdata_table
		if (!userdata->IsWithLuaTable())
		{
			userdata->SetWithLuaTable(true);
			lua_newtable(l);	
								// userdata_table usertable
			lua_pushlightuserdata(l, userdata.get());
								// userdata_table usertable userdata
			lua_pushvalue(l, -2);
								// userdata_table usertable userdata usertable
			lua_settable(l, -4);
								// userdata_table usertable
		}
		else
		{
			lua_pushlightuserdata(l, userdata.get());
								// userdata_table userdata
			lua_gettable(l, -2);
		}
								// userdata_table usertable
		Debug::CheckAssertion(!lua_isnil(l, -1), "userdata tables is not init.");
				
								// userdata_table userdata table
		lua_pushvalue(l, 2);
								// userdata_table userdata table key
		lua_pushvalue(l, 3);
								// userdata_table userdata table key value
		lua_settable(l, -3);

		// 将xxx=yyy的key值保存在userdataFields中
		if (lua_isstring(l, 2))
		{
			if (!lua_isnil(l, 3))
			{
				GetLuaContext(l).mUserdataFields[userdata.get()].insert(LuaTools::CheckString(l, 2));
			}
			else
			{
				GetLuaContext(l).mUserdataFields[userdata.get()].erase(LuaTools::CheckString(l, 2));

			}
		}

		return 0;	
	});
}

/**
*	\biref lua index元方法处理存储特殊数据
*/
int LuaContext::userdata_meta_index(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaTools::CheckType(l, 1, LUA_TUSERDATA);
		LuaTools::CheckAny(l, 2);

		LuaContext& luaContext = GetLuaContext(l);
		//luaContext.PrintLuaStack(l);
		const LuaObjectPtr& userdata = *static_cast<LuaObjectPtr*>(lua_touserdata(l, 1));
		if (userdata->IsWithLuaTable() && lua_isstring(l, 2))
		{	
			// 从userdata table中
			lua_getfield(l, LUA_REGISTRYINDEX, "userdata_tables");
			Debug::CheckAssertion(!lua_isnil(l, -1), "userdatas tables is not init.");
						// ... utabel
			lua_pushlightuserdata(l, userdata.get());
						// ... utable userdata
			lua_rawget(l, -2);			// 不用lua_gettable(l, -2)，因为不需要从元表中找
						// ... utabel table/uil
			if (!lua_isnil(l, -1))
			{
						// ... utabel table
				lua_pushvalue(l, 2);
				lua_gettable(l, -2);
						// ... utabel table value/nil
				if (!lua_isnil(l, -1))
				{
					return 1;
				}
			}
		}
		// 从metatable中找
		lua_pushvalue(l, 1);
						// ... userdata
		lua_getmetatable(l, -1);
						// ... userdata meta
		if (lua_isnil(l, -1))
		{
			LuaTools::Error(l, "Invalid userdata without metatable.");
			lua_pop(l, 1);
			return 0;
		}
		lua_pushvalue(l, 2);
						// ... userdata meta key
		lua_gettable(l, -2);
						// ... userdata meta key value/nil
		return 1;
	});
}

/**
*	\brief 响应userdata销毁
*
*	当userdata(shared_ptr)的引用计数为0时，析构函数调用该函数,主要用来释放
*	自身表所保存的数据,同时从userda_fields容器中移除userdata
*/
void LuaContext::NotifyUserdataDestoryed(LuaObject& obj)
{
	if (obj.IsWithLuaTable())
	{
		lua_getfield(l, LUA_REGISTRYINDEX, "userdata_tables");	
								// userdata_table/nil
		if (!lua_isnil(l, -1))
		{
			lua_pushlightuserdata(l, &obj);
								// userdata_tables lightuserdata
			lua_pushnil(l);
								// userdata_tables lightuserdata nil
			lua_settable(l, -3);
								// userdata_talbes
		}
		lua_pop(l, 1);

		GetLuaContext(l).mUserdataFields.erase(&obj);
	}
}

/**
*	\brief 注册元表函数
*	\param moduleName 模块\类名
*	\param metaKey 注册的元表关键字
*	\param function 绑定的函数
*
*	绑定元表函数，如果元表不存在则创建一个新的元表
*/
void LuaContext::RegisterMetaFunction(lua_State * l, const string & moduleName, const std::string & key, FunctionExportToLua function)
{
	luaL_getmetatable(l, moduleName.c_str());
	if (lua_isnil(l, -1))
	{
		lua_pop(l, 1);
		luaL_newmetatable(l, moduleName.c_str());
	}
	lua_pushcfunction(l, function);
	lua_setfield(l, -2, key.c_str());
	lua_pop(l, 1);
}

/**
*	\brief 将函数注册在全局表的CAPI上
*/
void LuaContext::RegisterFunction(lua_State*l, const std::string& funcName, FunctionExportToLua function)
{
	lua_rawgeti(l, LUA_REGISTRYINDEX, mSystemCApiRef);
	if (!lua_isnil(l, -1) && function != nullptr)
	{
		lua_pushcfunction(l, function);
		lua_setfield(l, -2, funcName.c_str());
		lua_pop(l, 1);
	}
	lua_pop(l, 1);
}

/**
*	\brief 注册C++ API 函数
*
*	获取moduleName的global表，将函数绑定在表上
*/
void LuaContext::RegisterFunction(lua_State * l, const string & moduleName, const luaL_Reg * functions)
{
	if (!moduleName.empty())
	{
		lua_getglobal(l, module_name.c_str());
		// cjing
		lua_getfield(l, -1, moduleName.c_str());
		// cjing module/nil
		if (lua_isnil(l, -1))
		{
			lua_pop(l, 1);
			lua_newtable(l);
			// cjing module
		}
		if (functions != nullptr)
			luaL_setfuncs(l, functions, 0);
		lua_setfield(l, -2, moduleName.c_str());
		// cjing
		lua_pop(l, 1);
		// --
	}
	else
	{
		luaL_setfuncs(l, functions, 0);
	}
}

/**
*	\brief 注册一个C++对象类型
*/

void LuaContext::RegisterType(lua_State * l, const string & moduleName, const luaL_Reg * functions, const luaL_Reg * methods, const luaL_Reg * metamethods) 
{
	luaL_getmetatable(l, moduleName.c_str());
	Debug::CheckAssertion(lua_isnil(l, -1), string("Type:") + moduleName + " has already registered.");

	// 注册函数
	if (functions != nullptr )
	{
		RegisterFunction(l, moduleName, functions);
	}

	luaL_newmetatable(l, moduleName.c_str());
	lua_pushstring(l, moduleName.c_str());	
							// meta moduleName
	lua_setfield(l, -2, "__CJING_TYPE");
							// meta
	// 注册方法
	if (methods != nullptr )
	{
		RegisterFunction(l, "", methods);
	}
	// 注册元表方法
	if (metamethods != nullptr)
	{
		RegisterFunction(l, "", metamethods);
	}						// meta
	lua_getfield(l, -1, "__index");
							// meta index/nil
	lua_pushvalue(l, -2);
							// meta __index/nil 
	if ( lua_isnil(l, -2))
	{							
		lua_setfield(l, -3, "__index");
	}						// meta nil
	lua_settop(l, 0);
							// --
}

/**
*	\brief 注册一个C++对象类型，并设置该类型的基类
*
*	在使用该方法前必须保证baseModule存在，并已经注册
*	bug:目前使用该方法会导致父类metatable的gc方法调用
*/
void LuaContext::RegisterType(lua_State*l, const string& moduleName, const string& baseModuleName, 
		const luaL_Reg* functions, const luaL_Reg* methods, const luaL_Reg* metamethods)
{
	// base module 必须已经注册
	luaL_getmetatable(l, baseModuleName.c_str());
	Debug::CheckAssertion(!lua_isnil(l, -1), string("Base Type:") + baseModuleName + " has not registered.");
	lua_pop(l, 1);
				// --
	RegisterType(l, moduleName, functions, methods, metamethods);

	luaL_getmetatable(l, moduleName.c_str());
				// meta
	//lua_newtable(l);
	//			// meta table
	luaL_getmetatable(l, baseModuleName.c_str());
	//			// meta table basemeta
	//lua_setfield(l, -2, "__index");
	//			// meta table
	lua_setmetatable(l, -2);
				// meta
	luaL_getmetatable(l, baseModuleName.c_str());
				// meta basemeta
	lua_setfield(l, -2, "super");
	lua_settop(l, 0);
}

/**
*	\brief 为栈顶元素创建一个新的LuaRef引用
*/
LuaRef LuaContext::CreateRef()
{
	return LuaTools::CreateRef(l);
}

/**
*	\brief 将LuaRef引用对象压栈
*
*	如果LuaRef为空，则压入nil
*/
void LuaContext::PushRef(lua_State * l, const LuaRef & luaref)
{
	if (luaref.IsEmpty())
	{
		lua_pushnil(l);
		return;
	}
	// need to judge these lua_State?
	luaref.Push();
}

/**
*	\brief 打印当前Lua栈信息
*/
void LuaContext::PrintLuaStack(lua_State * l)
{
	int stackSize = lua_gettop(l);
	std::ostringstream oss;
	oss << endl;
	for (int i = 1; i <= stackSize; i++)
	{
		switch (lua_type(l,i))
		{
		case LUA_TSTRING:
			oss << "\"" << lua_tostring(l, i) << "\"";
			break;
		case LUA_TBOOLEAN:
			oss << (lua_toboolean(l,i) ? "True" : "False");
			break;
		case LUA_TNUMBER:
			oss << (lua_tonumber(l, i));
			break;
		default:
			oss << lua_typename(l, lua_type(l,i));
			break;
		}
		oss << endl;
	}
	Logger::Debug(oss.str());
}

/**
*	\brief 压入用户对象数据
*	\param l lua指针
*	\param userData 
*/
void LuaContext::PushUserdata(lua_State * l, LuaObject & userData)
{
	lua_getfield(l, LUA_REGISTRYINDEX, "all_userdata");
										// all_userdata
	lua_pushlightuserdata(l, &userData);
										// all_userdata  lightuser
	lua_gettable(l, -2);
										// all_userdata userdata/nil
	if (!lua_isnil(l, -1))
	{
		lua_remove(l, -2);
										// userdata
	}
	else
	{
		if (!userData.IsKnowToLua())
		{
			userData.SetKnowToLua(true);
			userData.SetLuaContext(&GetLuaContext(l));
		}
										// all_userdata nil
		lua_pop(l, 1);
		lua_pushlightuserdata(l, &userData);
										// all_userdata lightuser
		LuaObjectPtr luaObjectPtr = userData.shared_from_this();

		LuaObjectPtr* luaObjectPtrAddress = static_cast<LuaObjectPtr*>(lua_newuserdata(l, sizeof(LuaObjectPtr)));
		new (luaObjectPtrAddress)LuaObjectPtr(luaObjectPtr);
										// all_userdata lightuser userdata
		// 为对象添加元表
		luaL_getmetatable(l, userData.GetLuaObjectName().c_str());
										// all_userdata lightuser userdata mt
		// 保证存在元表,且元表中存在gc
		lua_setmetatable(l, -2);
										// all_userdata lightuser userdata
		lua_pushvalue(l, -1);
		lua_insert(l, -4);	
										// userdata all_userdata light userdata
		lua_settable(l, -3);
										// userdata all_userdata
		lua_pop(l, 1);
	}
}



/**
*	\brief 检查指定位置的lua用户对象，返回该对象
*/
const LuaObjectPtr LuaContext::CheckUserdata(lua_State * l, int index, const string & moduleName)
{
	index = LuaTools::GetPositiveIndex(l, index);
	
	/*const LuaObjectPtr& userdata = *(static_cast<LuaObjectPtr*>(luaL_checkudata(l, index, moduleName.c_str()) ));
	return userdata;*/

	// 临时处理派生类，目前的做法对非法情况直接抛出错误！！！
	void* udata = lua_touserdata(l, index);
	if (udata == nullptr)
	{
		LuaTools::ArgError(l, index, string("CheckUserdata():excepted userdata, but get") + lua_typename(l, index));
		return nullptr;
	}
	lua_getfield(l, LUA_REGISTRYINDEX, moduleName.c_str());
	if (lua_isnil(l, -1))
	{
		LuaTools::ArgError(l, index, string("CheckUserdata():unregistered type:") + moduleName);
		return nullptr;

	}					// dstmeta
	if (!lua_getmetatable(l, index))
	{
		LuaTools::ArgError(l, index, string("CheckUserdata():excepted userdata, but get") + lua_typename(l, index));
		return nullptr;
	}					// dstmeta curmeta

	// 这里需要考虑当前udata可能是module的派生类，通过获取
	// 元表的元表进行匹配来判断是否是子类,同时设置最大的继承
	// 深度
	const int MaxDeriveDepth = 5;
	int curDeriveDepth = 1;

	bool isMatched = false;
	int curIndex = LuaTools::GetPositiveIndex(l, -1);
	while (!lua_isnil(l, curIndex))
	{
		if (curDeriveDepth >= MaxDeriveDepth)
		{
			LuaTools::ArgError(l, index, 
				string("CheckUserdata():can not mathced current modulename,beyond max derive depths ") + moduleName);
			return nullptr;
		}

		isMatched = lua_rawequal(l, -1, -2);
		//GetLuaContext(l).PrintLuaStack(l);
		if (isMatched)
		{				// dstmeta meta
			lua_pop(l, 1);
						// dstmeat
			break;
		}
		else
		{				// dstmeta curmeta
			lua_getmetatable(l, -1);
			//GetLuaContext(l).PrintLuaStack(l);
						// dstmeta curmeta meta
			int removeIndex = LuaTools::GetPositiveIndex(l, -2);
			lua_remove(l, removeIndex);
			//GetLuaContext(l).PrintLuaStack(l);
						// dstmeta meta
		}
	}
	lua_pop(l, 1);
						// ...
	//GetLuaContext(l).PrintLuaStack(l);
	if (isMatched)
	{
		return *(static_cast<LuaObjectPtr*>(udata));
	}
	else
	{
		LuaTools::ArgError(l, index, string("CheckUserdata():can not mathced current modulename ") + moduleName);
		return nullptr;
	}
}

/**
*	\brief 检查指定位置的lua用户对象
*	\return true 如果对象为name类型userdata
*/
const bool LuaContext::IsUserdata(lua_State * l, int index, const string& name)
{
	// 先检查udata是否存在,其次metatable是否存在，然后对比metatalbe
	index = LuaTools::GetPositiveIndex(l, index);

	void* udata = lua_touserdata(l, index);
	if (udata == nullptr)
	{
		return false;
	}
	if (!lua_getmetatable(l, index))
	{
		return false;
	}
						// udata mt 
	lua_getfield(l, LUA_REGISTRYINDEX, name.c_str());
						// udata mt mt_excepted
	bool result = lua_rawequal(l, -1, -2);
	lua_pop(l, 2);
						// udata
	return result;
}

/**
*	\brief 检测userdata的userdata_tables中是否存在指定值
*/
bool LuaContext::IsUserdataHasField(LuaObject & userdata, const std::string & fieldName)
{
	// 是否是已注册数据
	if (!userdata.IsKnowToLua())
	{
		return false;
	}

	// 检查元表
	luaL_getmetatable(l, userdata.GetLuaObjectName().c_str());
							// metatable
	lua_pushstring(l, fieldName.c_str());
							// metatable fieldname
	lua_rawget(l, -2);
							// metatable nil/field
	bool foundInMeta = !lua_isnil(l, -1);
	lua_pop(l, 2);
	if (foundInMeta)
	{
		return true;
	}

	// 不包含表则直接返回
	if (!userdata.IsWithLuaTable())
	{
		return false;
	}

	// 从自定义数据表中查找
	const auto& it = mUserdataFields.find(&userdata);
	if (it == mUserdataFields.end())
	{
		return false;
	}
	return it->second.find(fieldName) != it->second.end();
}

/**
*	\brief 清楚所有Userdata数据
*/
void LuaContext::CloseUserdatas()
{
	lua_getfield(l,LUA_REGISTRYINDEX,"all_userdata");

	// 清楚userdata_tables数据
	mUserdataFields.clear();
	lua_pushnil(l);
	lua_setfield(l, LUA_REGISTRYINDEX, "userdata_tables");
}

void LuaContext::OnStart()
{
	if (FindMethod("onStarted"))
		LuaTools::CallFunction(l, 1, 0, "onStarted");
}

void LuaContext::OnUpdate()
{
	if (FindMethod("onUpdated"))
		LuaTools::CallFunction(l, 1, 0, "onUpdated");
}

void LuaContext::OnFinish()
{
	if (FindMethod("onFinished"))
		LuaTools::CallFunction(l, 1, 0, "onFinished");
}

void LuaContext::OnDraw()
{
	if (FindMethod("onDraw"))
	{
		LuaTools::CallFunction(l, 1, 0, "onDraw");
	}
}

void LuaContext::OnCreated()
{
	if (FindMethod("onCreated"))
	{
		LuaTools::CallFunction(l, 1, 0, "onCreated");
	}
}

/**
*	\brief 输入事件响应
*
*	根据event的类型(type和state)调用对应的Lua函数
*/
bool LuaContext::OnInput(const InputEvent & event)
{
	bool handle = false;
	if (event.IsKeyBoardEvent())
	{	// 键盘事件
		if (event.IsKeyBoardPressed())
			handle = OnKeyPressed(event);
		if (event.IsKeyBoardReleased())
			handle = OnKeyReleased(event);
	}
	else if (event.IsMouseEvent())
	{	// 鼠标事件
		if (event.GetEventType() == InputEvent::EVENT_MOUSE_MOTION)
		{
			handle = OnMouseMotion(event);
		}
	}		
	return handle;;
}

/**
*	\brief 响应键盘按下事件
*   \return 返回值handle如果为true，则表示事件已经结束无需传递
*
*	onKeyPressed(key,modifier),传入3个参数self为调用者key按下的键值,
*   modifier是否同时按下ctrl/shift/alt lua的函数返回值表示是否是一个
*   可传递的有效事件
*/
bool LuaContext::OnKeyPressed(const InputEvent & event)
{
	bool handle = false;
	if (FindMethod("onKeyPressed"))	// 注意FindMethod会将Ojb复制一份放于stack中
	{								// obj method obj
		string key = EnumToString(event.GetKeyBoardKey(), "");
		if (!key.empty())
		{
			lua_pushstring(l, key.c_str());
			lua_newtable(l);	// 创建modifiers表，用于设置是否同时按下其他修饰按键（主要包括
								// ctrl、alt、shift)
			if (event.IsWithKeyShift())
			{
				lua_pushboolean(l, 1);
				lua_setfield(l, -2, "shift");
			}
			if (event.IsWithKeyAlt())
			{
				lua_pushboolean(l, 1);
				lua_setfield(l, -2, "alt");
			}
			if (event.IsWithKeyCtrl())
			{
				lua_pushboolean(l, 1);
				lua_setfield(l, -2, "ctrl");
			}
			bool success = LuaTools::CallFunction(l, 3, 1, "onKeyPressed");
			if (!success)
				handle = true;
			else
			{
				handle = lua_toboolean(l, -1);
				lua_pop(l, 1);
			}
		}
		else// obj mothod obj
			lua_pop(l, 2);
	}
	return handle;
}

/**
*	\brief 响应键盘放开事件
*/
bool LuaContext::OnKeyReleased(const InputEvent & event)
{
	bool handle = false;
	if (FindMethod("onKeyReleased"))// 注意FindMethod会将Ojb复制一份放于stack中
	{								// obj method obj
		string key = EnumToString(event.GetKeyBoardKey(), "");
		if (!key.empty())
		{
			lua_pushstring(l, key.c_str());
			lua_newtable(l);// modifiers

			if (event.IsWithKeyAlt())
			{
				lua_pushboolean(l, 1);
				lua_setfield(l, -2, "alt");
			}
			if (event.IsWithKeyShift())
			{
				lua_pushboolean(l, 1);
				lua_setfield(l, -2, "shift");
			}
			if (event.IsWithKeyCtrl())
			{
				lua_pushboolean(l, 1);
				lua_setfield(l, -2, "ctrl");
			}
			bool success = LuaTools::CallFunction(l, 3, 1, "onKeyReleased");
			if (!success)
				handle = true;
			else
			{
				handle = lua_toboolean(l, -1);
				lua_pop(l, 1);
			}
		}
		else // obj method obj
			lua_pop(l, 2);
	}
	return handle;
}

/**
*	\brief 响应鼠标移动事件
*/
bool LuaContext::OnMouseMotion(const InputEvent & event)
{
	bool handle = false;
	if (FindMethod("onMouseMotion"))// 注意FindMethod会将Ojb复制一份放于stack中
	{								// obj method obj
		const Point2& mousePos = event.GetKeyEvent().motion;
		lua_pushinteger(l, mousePos.x);
		lua_pushinteger(l, mousePos.y);

		bool success = LuaTools::CallFunction(l, 3, 1, "onMouseMotion");
		if (!success)
			handle = true;
		else
		{
			handle = lua_toboolean(l, -1);
			lua_pop(l, 1);
		}
		
	}
	return handle;
}

/**
*	\brief 注册lua接口
*
*	对于有派生关系的module，需要保证正确的注册顺序
*/
void LuaContext::RegisterModules()
{
	RegisterMainModule();
	RegisterSoundModule();
	RegisterGameModule();
	RegisterMapModule();
	RegisterVideoModule();
	RegisterFontModule();
	RegisterMenuModule();
	RegisterTimeModule();
	RegisterSpriteModule();
	RegisterAnimationModule();
	RegisterMovementModule();
	RegisterAsyncLoaderModule();
	RegisterEntityModule();
	RegisterParticle();
	RegisterItem();
	//RegisterWindowModule();
}

LuaContext& LuaContext::GetLuaContext(lua_State* l)
{
	return *mLuaContexts[l];
}

/**
*	\brief unprotected lua error发生时调用
*/
int LuaContext::api_panic(lua_State*l)
{
	string errmsg = LuaTools::CheckString(l, -1);
	Debug::Die(errmsg);
	return 0;
}