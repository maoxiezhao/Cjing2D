#include"luaContext.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"core\logger.h"
#include"game\app.h"
#include<sstream>

std::map<lua_State*, LuaContext*> LuaContext::mLuaContexts;
const string LuaContext::module_name = "cjing";

LuaContext::LuaContext(App* app):
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
	// 注册函数
	RegisterModules();

	// 自定义lua require 加载器，用于解析打包的资源文件


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
	UpdateMenus();

	OnMainUpdate();
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

		lua_close(l);
		mLuaContexts.erase(l);
		l = nullptr;
	}
}

App * LuaContext::GetApp() const
{
	return mApp;
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



/**
*	\brief 注册C++ API 函数
*
*	获取moduleName的global表，将函数绑定在表上
*/
void LuaContext::RegisterFunction(const string & moduleName, const luaL_Reg * functions)
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

void LuaContext::RegisterModules()
{
	RegisterMainModule();
	RegisterMenuModule();
	RegisterTimeModule();
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