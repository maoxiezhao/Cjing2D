#ifndef _LUACONTEXT_H_
#define _LUACONTEXT_H_

#include"common\common.h"
#include"lua\luaTools.h"
#include"lua\luaRef.h"
#include"lua\luaObject.h"
#include"game\timer.h"
#include"game\sprite.h"
#include"game\drawable.h"

#include<set>
#include<map>
#include<list>
#include<thirdparty\lua.hpp>

class App;
class InputEvent;
class Sprite;
class Drawable;
class AnimationSprite;

/**
 *	\brief C++和lua的接口，提供与用于lua使用的C++ API
 *
 *  其中的提供的scripts包括main,sprite,map,entity等
 *  其中所有的script运行在同一个luaContext
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
	bool LoadFile(lua_State*l, const string& name);
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
	bool OnInput(const InputEvent& event);
	bool OnKeyPressed(const InputEvent& event);
	bool OnKeyReleased(const InputEvent& event);

	// modules
	void RegisterModules();
	void RegisterMainModule();
	void RegisterTimeModule();
	void RegisterMenuModule();
	void RegisterVideoModule();
	void RegisterMovementModule();
	void RegisterSpriteModule();
	void RegisterAnimationModule();

	// binding function
	using FunctionExportToLua = int(lua_State* l);
	static FunctionExportToLua
		// error
		api_panic,
		// main
		main_api_hello,
		main_api_exit,
		// time
		time_api_start,
		// menu
		menu_api_start,
		menu_api_stop,
		// video
		video_api_setFullScreen,
		video_api_isFullScreen,
		// drawable
		drawable_api_get_pos,
		drawable_api_set_pos,
		drawable_meta_api_gc,
		// sprite
		sprite_api_create,
		sprite_api_set_size,
		sprite_api_get_size,
		sprite_api_set_blend,
		sprite_api_get_blend,
		sprite_api_set_rotation,
		sprite_api_get_rotation,
		sprite_api_run_movement,
		sprite_api_get_movement,
		sprite_api_stop_movment,
		sprite_api_set_opacity,
		sprite_api_get_opacity,
		sprite_api_draw,
		// animation
		animation_api_create,
		animation_api_set_animation,
		animation_api_set_direction,
		animation_api_play_animation,
		animation_api_stop_animation,
		animation_api_draw,
		// movement
		movement_api_create,
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
	void OnGameStart();
	void OnGameUpdate();
	void OnGameFinish();
	void OnGameDraw();
	bool OnGameInput(const InputEvent& event);

	// map api
	void OnMapStart();
	void OnMapUpdate();
	void OnMapFinish();
	void OnMapDraw();
	bool OnMapInput(const InputEvent& event);

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

	// push data
	static void PushUserdata(lua_State*l, LuaObject& userData);
	static void PushDrawable(lua_State*l, Drawable& drawable);
	static void PushSprite(lua_State*l, Sprite& sprite);
	static void PushAnimation(lua_State*l, AnimationSprite& animation);

	// checkXX and isXXX
	static DrawablePtr CheckDrawable(lua_State*l, int index);
	static bool IsDrawable(lua_State*l, int index);
	static SpritePtr CheckSprite(lua_State*l, int index);
	static bool IsSprite(lua_State*l, int index);
	static std::shared_ptr<AnimationSprite> CheckAnimation(lua_State*l, int index);
	static bool IsAnimation(lua_State*l, int index);

	// modules name
	static const string module_name;
	static const string module_main_name;
	static const string module_map_name;
	static const string module_time_name;
	static const string module_menu_name;
	static const string module_video_name;

	// movement modules name
	static const string module_movement_name;
	static const string module_straight_movment_name;

	// drawable
	static const string module_drawable_name;
	static const string module_sprite_name;
	static const string module_animation_name;

private:
	App* mApp;
	lua_State* l;
	static std::map<lua_State*, LuaContext*>mLuaContexts;/* 用于在lua APi中通过lua_state获取
														    luaContext */

	std::map<TimerPtr, TimerData>mTimers;				 /* 存储了定时器，每个定时器映射的对
														    应的callback*/
	std::list<TimerPtr>mTimersToRemove;					 

	std::list<MenuData>mMenus;							/* 存储了菜单，每个菜单存有映射menu 
															table的luaRef*/

	std::set<std::shared_ptr<Drawable> >mDrawables;						/* 存储了由脚本创建的Drawable对象*/
	std::set<std::shared_ptr<Drawable> >mDrawablesToRemove;

};

#endif