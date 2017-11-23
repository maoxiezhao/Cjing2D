#include"luaContext.h"
#include"core\video.h"
#include"core\fileData.h"
#include"core\resourceCache.h"
#include"thirdparty\SOIL.h"

const string LuaContext::module_video_name = "Video";

/**
*	\brief ע��videoģ��
*/
void LuaContext::RegisterVideoModule()
{
	static const luaL_Reg functions[] = {
		{"setFullScreen",video_api_setFullScreen},
		{"isFullScreen",video_api_isFullScreen},
		{"getScreenSize", video_api_get_wanted_size},
		{"getFPS", video_api_get_fps },
		{"setCursor", video_api_set_cursor },
		{"loadProgram", video_api_load_program},
		{nullptr,nullptr}
	};
	RegisterFunction(l, module_video_name, functions);
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

/**
*	\brief ���ص�ǰ���ڴ�С
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
*	\brief ���ص�ǰ֡��
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
*	\brief ���õ�ǰcursor,cjing.Video.setCursor(cursorImgPath)
*/
int LuaContext::video_api_set_cursor(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& cursorImgPath = LuaTools::CheckString(l, 1);

		// ������Debug::Error�ķ�ʽ�����ļ�ȱʧ����
		if (!FileData::IsFileExists(cursorImgPath))
		{
			LuaTools::Error(l, "Can not find file:" + cursorImgPath);
			return 0;
		}
		// �����ͼƬ�Ķ�ȡ��ʹ��texture,��Ϊ����Ը���Դ����ά��
		int w, h;
		const string data = FileData::ReadFile(cursorImgPath);
		unsigned char* imageData = SOIL_load_image_from_memory((unsigned char*)data.c_str(), 
				data.length(), &w, &h, 0, SOIL_LOAD_RGBA);

		Video::SetImageCursor(imageData, w, h);
		return 0;
	});
}


/**
*	\brief ����һ���µ�program,cjing.Video.loadProgram(keyname, vertexName, fragName)
*
*	Ŀǰ���ڼ򻯵Ŀ��ǣ�������һ���µ�programʱ��Ҳ��Ĭ�ϴ���һ��Ψһ��programState
*/
int LuaContext::video_api_load_program(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& keyName = LuaTools::CheckString(l, 1);
		const std::string& vertexFileName = LuaTools::CheckString(l, 2);
		const std::string& fragFileName = LuaTools::CheckString(l, 3);

		// ����program
		auto program = std::make_shared<GLProgram>();
		program->InitWithFileNames(vertexFileName, fragFileName);
		ResourceCache::GetInstance().AddGLProgram(program, keyName);

		// ����programState
		auto programState = std::make_shared<GLProgramState>();
		programState->Set(program);
		ResourceCache::GetInstance().AddGLProgramState(programState, keyName);

		return 0;
	});
}