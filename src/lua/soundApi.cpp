#include"luaContext.h"
#include"luaTools.h"
#include"core\music.h"
const string LuaContext::module_sound_name = "Sound";

void LuaContext::RegisterSoundModule()
{
	static const luaL_Reg functions[] = {
		{ "playMusic",sound_api_play_music },
		{ "stopMusic",sound_api_stop_music },
		{ nullptr,nullptr }
	};
	RegisterFunction(l, module_sound_name, functions);
}

/**
*	\brief 实现cjing.Sound.playMusic(filePath, looped)
*/
int LuaContext::sound_api_play_music(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&]{
		const std::string& filePath = LuaTools::CheckString(l, 1);
		bool looped = LuaTools::CheckBoolean(l, 2);
		Music::PlayMusic(filePath, looped);
		return 0;
	});
}

/**
*	\brief 实现cjing.Sound.stopMusic()
*/
int LuaContext::sound_api_stop_music(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Music::StopMusic();
		return 0;
	});
}