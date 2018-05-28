#include"luaContext.h"
#include"luaTools.h"
#include"lua\luaBinder.h"
#include"core\music.h"
#include"core\sound.h"

const string LuaContext::module_sound_name = "Sound";

void LuaContext::RegisterSoundModule()
{
	LuaBindClass<Sound> soundClass(l, module_sound_name);
	soundClass.AddFunction("PlayMusic", sound_api_play_music);
	soundClass.AddFunction("StopMusic", sound_api_stop_music);
	soundClass.AddFunction("LoadSound", sound_api_load_sound);
	soundClass.AddFunction("PlaySound", sound_api_play_sound);
	soundClass.AddFunction("SetMusicVolume", &Music::SetMusicVolume);
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

/**
*	\brief 实现cjing.Sound.stopMusic()
*/
int LuaContext::sound_api_load_sound(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
	
		return 0;
	});
}

/**
*	\brief 实现cjing.Sound.stopMusic()
*/
int LuaContext::sound_api_play_sound(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		const std::string& id = LuaTools::CheckString(l, 1);
		Sound::PlaySound(id);

		return 0;
	});
}
