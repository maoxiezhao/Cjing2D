#include"luaContext.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"core\logger.h"
#include"game\app.h"
#include"game\enumInfo.h"
#include<sstream>

/**
*	\brief ��ʼ��lua�������ַ�����
*/
const std::string luaEnvInitScript =
// ��ʼ��lua�ű�����
"SystemEnvTables = {__G__ = _ENV}\n"
"SystemEnvTables.Enum = {}\n"
"SystemEnvTables.Const = SystemEnvTables.Enum\n"
"SystemEnvTables.CApi = {}\n"
"SystemEnvTables.Modules = {}\n"
"SystemEnvTables.Imports = {}\n"
"\n"
"Global_Exports = setmetatable({}, {__index = _ENV, __newindex=function(t,k,v)rawset(_ENV, k, v)  end})\n"
"setmetatable(_ENV, {__index = SystemEnvTables.CApi })\n"
// ��ʼ��lua��������
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
// ��ʼ���ļ�ִ�л�������
"SystemDoFile = function(script, env, ...)\n"
//"local fileData = FileData.GetFileData()"
//"local buf = fileData:Load(script)"
"local buf = nil\n"
"local f,err = load(buf, script, env or _ENV) \n"
"if f==nil then error(err) end\n"
"return f(...)"
"end\n"
"\n"
"local traceBack = debug.traceback\n"
"local _logger = nil\n"
"SstemLogErrHandle = function(err)err = _traceback(err,2);return err; end\n"
"SystemTraceDoFile - function(script, env, ...)\n"
"local ok, res = xpcall(SystemDoFile, SystemLogErrHandle, script, env or _ENV, ...)\n"
"if ok then return res end\n"
"end\n"
// ��ʼ��lua modules����
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
" SystemImport = function(name)\n"
"  local i = imports[name]\n"
"  if not i then\n"
"   local m = __script_system_module(name)\n"
"   i = setmetatable({}, {__index=function(t,k) return rawget(m,k)end, __newindex=function(t,k,v) error('module read only') end })\n"
"   imports[name] = i\n"
"  end\n"
"  return i\n"
" end\n"
"end\n"
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
*	\brief ��ʼ��lua
*
*	������ʼ��lua_state��ע�����ģ�飬����userdata table
*/
void LuaContext::Initialize()
{
	l = luaL_newstate();
	lua_atpanic(l, api_panic);
	luaL_openlibs(l);
	mLuaContexts[l] = this;

	// ��ʼ��userdata����
	InitUserdataEnv(l);

	// ��ʼ��lua�ļ�ϵͳ��ע��
	RegisterFileData(l);

	// �Զ���lua require �����������ڽ����������Դ�ļ�
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


	// ����ȫ�ֺ���, ׼��������ȫ�ֺ�����luaEnvScript�ж���
	DoFileIfExists(l, "script/libFunction");

	// ע��ģ��
	RegisterModules();
	
	// ����main�ű�,����start����ʼִ��
	Debug::CheckAssertion(lua_gettop(l) == 0, "The lua stack is not empty.");
	DoFileIfExists(l,"main");
	Debug::CheckAssertion(lua_gettop(l) == 0, "The lua stack is not empty.");

	OnMainStart();
}

/**
*	\brief ˢ��Lua
*
*	����ˢ��timers,menu,drawable,movements
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
		// ���첽���صĹ����п��ܻ����
		// nil,��������ʱ����취
		lua_settop(l, 0);
	}


	Debug::CheckAssertion(lua_gettop(l) == 0,
		"There are something in lua stack after update");
}


/**
*	\brief �ر�lua
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
*	\brief ��Ӧ�����¼�
*
*   ����OnMainInput��������mainȥ�����¼�
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
	if (luaL_loadstring(l, luaString.c_str()) != 0)
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
*	\brief ����һ��lua�ļ�������ļ�����������ջ��
*/
bool LuaContext::LoadFile(lua_State* l, const string& name)
{
	string fileName(name);
	// ��������ڣ���ĩβ����Lua���ٳ���
	if(!FileData::IsFileExists(fileName))
		fileName += ".lua";

	// ������ɲ������򷵻�
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
*	\brief ����ָ���������ķ�������������򽫷�������ջ��
*
*	\return������������ڣ�����false
*/
bool LuaContext::FindMethod(const string & name, int index)
{
	int positiveIndex = LuaTools::GetPositiveIndex(l,index);	// ��þ�������
	
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
	lua_pushvalue(l, positiveIndex);// objectҪ��Ϊ����
							// -- object -- function object
	return true;
}

void LuaContext::InitUserdataEnv(lua_State* l)
{
	// ����userdata ��,����ģʽΪ����
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

	// ����userdata_table
	lua_newtable(l);
	lua_setfield(l, LUA_REGISTRYINDEX, "userdata_tables");

	lua_newtable(l);
	lua_setglobal(l, module_name.c_str());
}

/**
*	\brief userdata���ս���
*/
int LuaContext::userdata_meta_gc(lua_State*l)
{
	LuaObjectPtr* luaObject = static_cast<LuaObjectPtr*>(lua_touserdata(l, 1));
	if (luaObject != nullptr)		// �пյĴ����ڴ�й¶�Ŀ���
		luaObject->~shared_ptr<LuaObject>();
	return 0;
}

/**
*	\biref lua newindexԪ��������洢��������
*
*	xxx = yyyy,��ônewindex�����Ὣ3��������userdata,xxx,yyyѹջ,
*	���userdata_tables���Ѿ�����tables����ֱ�Ӵ�ȡ����Ȼ
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

		// ��xxx=yyy��keyֵ������userdataFields��
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
*	\biref lua indexԪ��������洢��������
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
			// ��userdata table��
			lua_getfield(l, LUA_REGISTRYINDEX, "userdata_tables");
			Debug::CheckAssertion(!lua_isnil(l, -1), "userdatas tables is not init.");
						// ... utabel
			lua_pushlightuserdata(l, userdata.get());
						// ... utable userdata
			lua_rawget(l, -2);			// ����lua_gettable(l, -2)����Ϊ����Ҫ��Ԫ������
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
		// ��metatable����
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
*	\brief ��Ӧuserdata����
*
*	��userdata(shared_ptr)�����ü���Ϊ0ʱ�������������øú���,��Ҫ�����ͷ�
*	����������������,ͬʱ��userda_fields�������Ƴ�userdata
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
*	\brief ע��Ԫ����
*	\param moduleName ģ��\����
*	\param metaKey ע���Ԫ��ؼ���
*	\param function �󶨵ĺ���
*
*	��Ԫ���������Ԫ�������򴴽�һ���µ�Ԫ��
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
*	\brief ������ע����ȫ�ֱ��CAPI��
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
*	\brief ע��C++ API ����
*
*	��ȡmoduleName��global�����������ڱ���
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
*	\brief ע��һ��C++��������
*/

void LuaContext::RegisterType(lua_State * l, const string & moduleName, const luaL_Reg * functions, const luaL_Reg * methods, const luaL_Reg * metamethods) 
{
	luaL_getmetatable(l, moduleName.c_str());
	Debug::CheckAssertion(lua_isnil(l, -1), string("Type:") + moduleName + " has already registered.");

	// ע�ắ��
	if (functions != nullptr )
	{
		RegisterFunction(l, moduleName, functions);
	}

	luaL_newmetatable(l, moduleName.c_str());
	lua_pushstring(l, moduleName.c_str());	
							// meta moduleName
	lua_setfield(l, -2, "__CJING_TYPE");
							// meta
	// ע�᷽��
	if (methods != nullptr )
	{
		RegisterFunction(l, "", methods);
	}
	// ע��Ԫ����
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
*	\brief ע��һ��C++�������ͣ������ø����͵Ļ���
*
*	��ʹ�ø÷���ǰ���뱣֤baseModule���ڣ����Ѿ�ע��
*	bug:Ŀǰʹ�ø÷����ᵼ�¸���metatable��gc��������
*/
void LuaContext::RegisterType(lua_State*l, const string& moduleName, const string& baseModuleName, 
		const luaL_Reg* functions, const luaL_Reg* methods, const luaL_Reg* metamethods)
{
	// base module �����Ѿ�ע��
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
*	\brief Ϊջ��Ԫ�ش���һ���µ�LuaRef����
*/
LuaRef LuaContext::CreateRef()
{
	return LuaTools::CreateRef(l);
}

/**
*	\brief ��LuaRef���ö���ѹջ
*
*	���LuaRefΪ�գ���ѹ��nil
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
*	\brief ��ӡ��ǰLuaջ��Ϣ
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
*	\brief ѹ���û���������
*	\param l luaָ��
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
		// Ϊ�������Ԫ��
		luaL_getmetatable(l, userData.GetLuaObjectName().c_str());
										// all_userdata lightuser userdata mt
		// ��֤����Ԫ��,��Ԫ���д���gc
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
*	\brief ���ָ��λ�õ�lua�û����󣬷��ظö���
*/
const LuaObjectPtr LuaContext::CheckUserdata(lua_State * l, int index, const string & moduleName)
{
	index = LuaTools::GetPositiveIndex(l, index);
	
	/*const LuaObjectPtr& userdata = *(static_cast<LuaObjectPtr*>(luaL_checkudata(l, index, moduleName.c_str()) ));
	return userdata;*/

	// ��ʱ���������࣬Ŀǰ�������ԷǷ����ֱ���׳����󣡣���
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

	// ������Ҫ���ǵ�ǰudata������module�������࣬ͨ����ȡ
	// Ԫ���Ԫ�����ƥ�����ж��Ƿ�������,ͬʱ�������ļ̳�
	// ���
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
*	\brief ���ָ��λ�õ�lua�û�����
*	\return true �������Ϊname����userdata
*/
const bool LuaContext::IsUserdata(lua_State * l, int index, const string& name)
{
	// �ȼ��udata�Ƿ����,���metatable�Ƿ���ڣ�Ȼ��Ա�metatalbe
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
*	\brief ���userdata��userdata_tables���Ƿ����ָ��ֵ
*/
bool LuaContext::IsUserdataHasField(LuaObject & userdata, const std::string & fieldName)
{
	// �Ƿ�����ע������
	if (!userdata.IsKnowToLua())
	{
		return false;
	}

	// ���Ԫ��
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

	// ����������ֱ�ӷ���
	if (!userdata.IsWithLuaTable())
	{
		return false;
	}

	// ���Զ������ݱ��в���
	const auto& it = mUserdataFields.find(&userdata);
	if (it == mUserdataFields.end())
	{
		return false;
	}
	return it->second.find(fieldName) != it->second.end();
}

/**
*	\brief �������Userdata����
*/
void LuaContext::CloseUserdatas()
{
	lua_getfield(l,LUA_REGISTRYINDEX,"all_userdata");

	// ���userdata_tables����
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
*	\brief �����¼���Ӧ
*
*	����event������(type��state)���ö�Ӧ��Lua����
*/
bool LuaContext::OnInput(const InputEvent & event)
{
	bool handle = false;
	if (event.IsKeyBoardEvent())
	{	// �����¼�
		if (event.IsKeyBoardPressed())
			handle = OnKeyPressed(event);
		if (event.IsKeyBoardReleased())
			handle = OnKeyReleased(event);
	}
	else if (event.IsMouseEvent())
	{	// ����¼�
		if (event.GetEventType() == InputEvent::EVENT_MOUSE_MOTION)
		{
			handle = OnMouseMotion(event);
		}
	}		
	return handle;;
}

/**
*	\brief ��Ӧ���̰����¼�
*   \return ����ֵhandle���Ϊtrue�����ʾ�¼��Ѿ��������贫��
*
*	onKeyPressed(key,modifier),����3������selfΪ������key���µļ�ֵ,
*   modifier�Ƿ�ͬʱ����ctrl/shift/alt lua�ĺ�������ֵ��ʾ�Ƿ���һ��
*   �ɴ��ݵ���Ч�¼�
*/
bool LuaContext::OnKeyPressed(const InputEvent & event)
{
	bool handle = false;
	if (FindMethod("onKeyPressed"))	// ע��FindMethod�ὫOjb����һ�ݷ���stack��
	{								// obj method obj
		string key = EnumToString(event.GetKeyBoardKey(), "");
		if (!key.empty())
		{
			lua_pushstring(l, key.c_str());
			lua_newtable(l);	// ����modifiers�����������Ƿ�ͬʱ�����������ΰ�������Ҫ����
								// ctrl��alt��shift)
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
*	\brief ��Ӧ���̷ſ��¼�
*/
bool LuaContext::OnKeyReleased(const InputEvent & event)
{
	bool handle = false;
	if (FindMethod("onKeyReleased"))// ע��FindMethod�ὫOjb����һ�ݷ���stack��
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
*	\brief ��Ӧ����ƶ��¼�
*/
bool LuaContext::OnMouseMotion(const InputEvent & event)
{
	bool handle = false;
	if (FindMethod("onMouseMotion"))// ע��FindMethod�ὫOjb����һ�ݷ���stack��
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
*	\brief ע��lua�ӿ�
*
*	������������ϵ��module����Ҫ��֤��ȷ��ע��˳��
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
*	\brief unprotected lua error����ʱ����
*/
int LuaContext::api_panic(lua_State*l)
{
	string errmsg = LuaTools::CheckString(l, -1);
	Debug::Die(errmsg);
	return 0;
}