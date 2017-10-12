#ifndef _LUACONTEXT_H_
#define _LUACONTEXT_H_

#include"common\common.h"
#include"lua\luaTools.h"
#include"lua\luaRef.h"
#include"lua\luaObject.h"
#include"game\timer.h"
#include"game\sprite.h"
#include"game\drawable.h"
#include"entity\entityData.h"

#include<set>
#include<map>
#include<list>
#include<thirdparty\lua.hpp>

class App;
class InputEvent;
class Sprite;
class Drawable;
class AnimationSprite;
class Movement;
class StraightMovement;
class TargetMovement;
class TextDrawable;
class AsyncLoader;
class Savegame;
class Game;
class Map;

/**
 *	\brief C++和lua的接口，提供与用于lua使用的C++ API
 *
 *  其中的提供的scripts包括main,sprite,map,entity等
 *  其中所有的script运行在同一个luaContext
 *
 *	2017.8.29 添加font接口
 *
 *	同时考虑未来优化绑定结构，能将绑定函数分离到各个模块
 *	或者使用luabind库
 */
class LuaContext
{
public:
	LuaContext(App* app);
	~LuaContext();

	// system
	void Initialize();
	void Update();
	void Exit();
	App* GetApp()const;
	bool NotifyInput(const InputEvent& event);

	// script
	static LuaContext& GetLuaContext(lua_State* l);
	bool DoFileIfExists(lua_State*l,const string& name);
	static bool LoadFile(lua_State*l, const string& name);
	bool FindMethod(const string& name);
	bool FindMethod(const string& name, int index);
	void RegisterFunction(const string& moduleName, const luaL_Reg* functions);
	void RegisterType(const string& moduleName, const luaL_Reg* functions, const luaL_Reg* methods, const luaL_Reg* metamethods);
	void RegisterType(const string& moduleName, const string& baseModuleName, const luaL_Reg* functions, const luaL_Reg* methods, const luaL_Reg* metamethods);
	LuaRef CreateRef();
	void PushRef(lua_State*l,const LuaRef& luaref);
	void PrintLuaStack(lua_State*l);
	
	// userdata
	static const LuaObjectPtr CheckUserdata(lua_State*l, int index, const string& moduleName);
	static const bool IsUserdata(lua_State*l, int index, const string& name);
	void CloseUserdatas();
	void NotifyUserdataDestoryed(LuaObject& obj);

	// process
	void OnStart();
	void OnUpdate();
	void OnFinish();
	void OnDraw();
	
	// input event
	bool OnInput(const InputEvent& event);
	bool OnKeyPressed(const InputEvent& event);
	bool OnKeyReleased(const InputEvent& event);
	bool OnMousePressed(const InputEvent& event);
	bool OnMouseReleased(const InputEvent& event);
	bool OnMouseMotion(const InputEvent& event);

	// modules
	void RegisterModules();
	void RegisterMainModule();
	void RegisterGameModule();
	void RegisterMapModule();
	void RegisterTimeModule();
	void RegisterMenuModule();
	void RegisterVideoModule();
	void RegisterMovementModule();
	void RegisterSpriteModule();
	void RegisterAnimationModule();
	void RegisterFontModule();
	void RegisterAsyncLoaderModule();
	void RegisterEntityModule();
	void RegisterSoundModule();

	// binding function
	using FunctionExportToLua = int(lua_State* l);
	static FunctionExportToLua
		// error
		api_panic,
		// main
		main_api_hello,
		main_api_exit,
		main_api_load_file,
		// game
		game_api_load,
		game_api_exists,
		game_api_start,
		game_api_save,
		game_api_set_value,
		game_api_get_value,
		game_api_get_life,
		// time
		time_api_start,
		// menu
		menu_api_start,
		menu_api_stop,
		// video
		video_api_setFullScreen,
		video_api_isFullScreen,
		video_api_get_wanted_size,
		video_api_get_fps,
		video_api_set_cursor,
		// drawable
		drawable_api_get_pos,
		drawable_api_set_pos,
		drawable_api_run_movement,
		drawable_api_get_movement,
		drawable_api_stop_movment,
		drawable_meta_api_gc,
		// sprite
		sprite_api_create,
		sprite_api_set_size,
		sprite_api_get_size,
		sprite_api_set_blend,
		sprite_api_get_blend,
		sprite_api_set_rotation,
		sprite_api_get_rotation,
		sprite_api_set_opacity,
		sprite_api_get_opacity,
		sprite_api_set_anchor,
		sprite_api_draw,
		// animation
		animation_api_create,
		animation_api_set_animation,
		animation_api_set_direction,
		animation_api_play_animation,
		animation_api_stop_animation,
		animation_api_set_size,
		animation_api_draw,
		// movement
		movement_api_create,
		movement_api_start,
		movement_api_stop,
		movement_api_set_pos,
		movement_api_get_pos,
		movement_api_api_gc,
		// straight movement
		movement_straight_api_create,
		movement_straight_api_set_speed,
		movement_straight_api_get_speed,
		movement_straight_api_set_angle,
		movement_straight_api_get_angle,
		movement_straight_api_set_max_distance,
		movement_straight_api_get_max_distance,
		// target movement
		movement_target_api_create,
		movement_target_api_set_target,
		movement_target_api_get_target,
		movement_target_api_set_speed,
		movement_target_api_get_speed,
		// text 
		text_api_create,
		text_api_load_font,
		text_api_load_font_default,
		text_api_load_font_default_en,
		text_api_set_font,
		text_api_get_font,
		text_api_set_text,
		text_api_get_text,
		text_api_set_color,
		text_api_get_color,
		text_api_set_size,
		text_api_get_size,
		text_api_set_line_height,
		text_api_set_letter_spacing,
		text_api_set_horizontal_align,
		text_api_draw,
		// asyncloader api
		async_loader_api_create,
		async_loader_api_add_task,
		async_loader_api_set_callback,
		async_loader_api_run,
		async_loader_meta_api_gc,
		// sound
		sound_api_play_music,
		sound_api_stop_music,
		// entity

		// entity create
		entity_api_create_title,
		entity_api_create_destimation,
		entity_api_create_dynamic_title,
		// userdata
		userdata_meta_gc,
		userdata_meta_newindex,
		userdata_meta_index
		;
		
	// main api	-- test 
	void PushMain(lua_State*l);
	void OnMainStart();
	void OnMainUpdate();
	void OnMainFinish();
	void OnMainDraw();
	bool OnMainInput(const InputEvent& event);

	// game api
	void OnGameStart(Game& game);
	void OnGameUpdate(Game& game);
	void OnGameFinish(Game& game);
	void OnGameDraw(Game& game);
	bool OnGameInput(Game& game, const InputEvent& event);

	// map api
	void RunMap(Map& map);
	void OnMapStart(Map& map);
	void OnMapUpdate(Map& map);
	void OnMapFinish(Map& map);
	void OnMapDraw(Map& map);
	bool OnMapInput(Map& map, const InputEvent& event);

	// time api
	struct TimerData
	{
		LuaRef callBackRef;
		const void* l;
	};
	void AddTimer(const TimerPtr& timer,int contextIndex,const LuaRef& callback);
	void CallTimerRef(const TimerPtr& timer);
	void UpdateTimers();
	void DestoryTimers();

	// menu api
	struct MenuData	
	{	// 菜单数据结构，用于指向菜单的table
		LuaRef menuRef;
		const void*l;
		bool recentlyAdd;
		MenuData(const LuaRef&menuref, const void*context) :
			menuRef(menuref), l(context), recentlyAdd(true) {}
	};
	void AddMenu(int contextIndex,const LuaRef& menuRef );
	void UpdateMenus();
	void DestoryMenus();
	void OnMenuStart(const LuaRef&menuRef);
	void OnMenuUpdate(const LuaRef&menuRef);
	void OnMenuFinish(const LuaRef&menuRef);
	bool OnMenuInput(int contextIndex,const InputEvent& event);
	bool OnMenuInput(const InputEvent& event, const LuaRef&menuRef);
	void OnMenuDraw(int contextIndex);
	void OnMenuDraw(const LuaRef& menuRef);
	void RemoveMenus(int contextIndex);
	void RemoveMenus();

	// sprite api
	void UpdateDrawables();
	void DestoryDrawables();
	void AddDrawable(const std::shared_ptr<Drawable>& drawable);
	bool HasDrawable(const std::shared_ptr<Drawable>& drawalbe);
	void RemoveDrawable(const std::shared_ptr<Drawable>& drawable);

	// asyncloader api
	void UpdateAsyncLoaders();
	void DestoryAsyncLoaders();
	void AddAsyncLoaders(const std::shared_ptr<AsyncLoader>& asyncLoader);
	bool HasAsyncLoader(const std::shared_ptr<AsyncLoader>& asyncLoader);
	void RemoveAsyncLoader(const std::shared_ptr<AsyncLoader>& asyncLoader);

	// entity api
	bool CreateEntity(const EntityData& entityData, Map& map);

	// push data
	static void PushUserdata(lua_State*l, LuaObject& userData);
	static void PushDrawable(lua_State*l, Drawable& drawable);
	static void PushSprite(lua_State*l, Sprite& sprite);
	static void PushAnimation(lua_State*l, AnimationSprite& animation);
	static void PushMovement(lua_State*l, Movement& movement);
	static void PushText(lua_State*l, TextDrawable& textDrawable);
	static void PushAsyncLoader(lua_State*l, AsyncLoader& asyncLoader);
	static void PushGame(lua_State*l, Savegame& saveGame);
	static void PushMap(lua_State*l, Map& map);

	// checkXX and isXXX
	static DrawablePtr CheckDrawable(lua_State*l, int index);
	static bool IsDrawable(lua_State*l, int index);
	static SpritePtr CheckSprite(lua_State*l, int index);
	static bool IsSprite(lua_State*l, int index);
	static std::shared_ptr<AnimationSprite> CheckAnimation(lua_State*l, int index);
	static bool IsAnimation(lua_State*l, int index);
	static std::shared_ptr<Movement> CheckMovement(lua_State*l, int index);
	static bool IsMovement(lua_State*l, int index);
	static std::shared_ptr<StraightMovement> CheckStraightMovement(lua_State*l, int index);
	static bool IsStraightMovement(lua_State*l, int index);
	static std::shared_ptr<TargetMovement> CheckTargetMovement(lua_State*l, int index);
	static bool IsTargetMovement(lua_State*l, int index);
	static std::shared_ptr<TextDrawable> CheckTextDrawable(lua_State*l, int index);
	static bool IsTextDrawable(lua_State*l, int index);
	static std::shared_ptr<AsyncLoader> CheckAsyncLoader(lua_State*l, int index);
	static bool IsAsyncLoader(lua_State*l, int index);
	static std::shared_ptr<Savegame> CheckSavegame(lua_State*l, int index);
	static bool IsSavegame(lua_State*l, int index);
	static std::shared_ptr<Map> CheckMap(lua_State*l, int index);
	static bool IsMap(lua_State*l, int index);


	// modules name
	static const string module_name;
	// base modules name
	static const string module_main_name;
	static const string module_game_name;
	static const string module_map_name;
	static const string module_time_name;
	static const string module_menu_name;
	static const string module_video_name;
	static const string module_font_name;
	static const string module_async_loader_name;
	static const string module_sound_name;
	// movement modules name
	static const string module_movement_name;
	static const string module_straight_movement_name;
	static const string module_target_movement_name;
	// drawable
	static const string module_drawable_name;
	static const string module_sprite_name;
	static const string module_animation_name;
	// entity
	static const string module_entity_name;

private:
	App* mApp;
	lua_State* l;
	static std::map<lua_State*, LuaContext*>mLuaContexts;/* 用于在lua APi中通过lua_state获取
														    luaContext */

	std::map<TimerPtr, TimerData>mTimers;				 /* 存储了定时器，每个定时器映射的对应的callback*/
	std::list<TimerPtr>mTimersToRemove;					 

	std::list<MenuData>mMenus;							/* 存储了菜单，每个菜单存有映射menu 
															table的luaRef*/

	std::set<std::shared_ptr<Drawable> >mDrawables;		/* 存储了由脚本创建的Drawable对象*/
	std::set<std::shared_ptr<Drawable> >mDrawablesToRemove;

	std::set<std::shared_ptr<AsyncLoader> >mAsyncLoaders;	/** 存储了异步加载器 */
	std::set<std::shared_ptr<AsyncLoader> >mAsyncLoaderToRemove;

	static std::map<EntityType, lua_CFunction> mEntitityCreaters;

};

#endif