#include"luaContext.h"
#include"core\video.h"
#include"core\fileData.h"
#include"thirdparty\SOIL.h"

const string LuaContext::module_video_name = "Video";

/**
*	\brief 注册video模块
*/
void LuaContext::RegisterVideoModule()
{
	static const luaL_Reg functions[] = {
		{"setFullScreen",video_api_setFullScreen},
		{"isFullScreen",video_api_isFullScreen},
		{"getScreenSize", video_api_get_wanted_size},
		{"getFPS", video_api_get_fps },
		{"setCursor", video_api_set_cursor },
		{nullptr,nullptr}
	};
	RegisterFunction(module_video_name, functions);
}


/**
*	\brief 设置当前是否全屏
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
*	\brief 返回当前是否全屏
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

/**
*	\brief 返回当前窗口大小
*/
int LuaContext::video_api_get_wanted_size(lua_State* l) 
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const Size& size = Video::GetScreenSize();
		lua_pushinteger(l, size.width);
		lua_pushinteger(l, size.height);

		return 2;
	});
}

/**
*	\brief 返回当前帧数
*/
int LuaContext::video_api_get_fps(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		uint32_t fps = Video::GetFPS();
		lua_pushinteger(l, fps);

		return 1;
	});
}

/**
*	\brief 设置当前cursor,cjing.Video.setCursor(cursorImgPath)
*/
int LuaContext::video_api_set_cursor(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& cursorImgPath = LuaTools::CheckString(l, 1);

		// 这里以Debug::Error的方式来对文件缺失报错
		if (!FileData::IsFileExists(cursorImgPath))
		{
			LuaTools::Error(l, "Can not find file:" + cursorImgPath);
			return 0;
		}
		// 这里对图片的读取不使用texture,因为无需对该资源后续维护
		int w, h;
		const string data = FileData::ReadFile(cursorImgPath);
		unsigned char* imageData = SOIL_load_image_from_memory((unsigned char*)data.c_str(), 
				data.length(), &w, &h, 0, SOIL_LOAD_RGBA);

		Video::SetImageCursor(imageData, w, h);
		return 0;
	});
}

