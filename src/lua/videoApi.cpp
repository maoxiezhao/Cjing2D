#include"luaContext.h"
#include"core\video.h"

const string LuaContext::module_video_name = "Video";

/**
*	\brief ע��videoģ��
*/
void LuaContext::RegisterVideoModule()
{
	static const luaL_Reg functions[] = {
		{"SetFullScreen",video_api_setFullScreen},
		{"IsFullScreen",video_api_isFullScreen},
		{nullptr,nullptr}
	};
	RegisterFunction(module_video_name, functions);
}


/**
*	\brief ���õ�ǰ�Ƿ�ȫ��
*/
int LuaContext::video_api_setFullScreen(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&]{
		LuaTools::CheckType(l, 1, LUA_TBOOLEAN);
		bool fullScreen = lua_toboolean(l, 1);
		Video::SetFullScreen(fullScreen);
		return 0;
	});
}

/**
*	\brief ���ص�ǰ�Ƿ�ȫ��
*/
int LuaContext::video_api_isFullScreen(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaTools::CheckType(l, 1, LUA_TNONE);
		bool fullScreen = Video::IsFullScreen();
		lua_pushboolean(l, fullScreen);
		return 1;
	});
}