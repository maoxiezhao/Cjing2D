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
#include<functional>

//#define NOT_DEFERRED_METATABLE	// 不使用多重元表

class LuaDebug;
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
class MapRoom;
class ParticleSystem;
class Item;
class ItemAcquired;
class Entity;
class Player;
class Weapon;

/**
 *	\brief C++和lua的接口，提供与用于lua使用的C++ API
 *
 *  其中的提供的scripts包括main,sprite,map,entity等
 *  其中所有的script运行在同一个luaContext
 *
 *  2018.4.30: 对于有继承关系的对象，将父方法直接写到子类
 *	元表中
 *	2017.12.13 调整luaENV，所有的注册使用的新的lua环境
 *	2017.11.22 以luaBinder形式注册模板
 *	2017.8.29  添加font接口
 *
 *	TODO：对于表进行优化操作，对于经常使用的模块方法，将
 *	其创建在一个全局表中（'.'用'_'代替）以更多的较少查询
 *	时间
 */
class LuaContext
{
public:
	/** Lua系统级函数索引 */
	enum SystemFunctionIndex {
		CLIENT_LUA_MAIN_START = 1,
		CLIENT_LUA_MAIN_UPDATE,
		CLIENT_LUA_MAIN_STOP,
		CLIENT_LUA_MAIN_RENDER,
		CLIENT_LUA_FIRE_EVENT,
		CLIENT_LUA_INPUT_KEY_DOWN,
		CLIENT_LUA_INPUT_KEY_UP,
	};

	/** Lua系统级事件，必须和Lua中的定义一致 */
	enum SystemLuaEvent {
		EVENT_GAME_MAP_ENTER = 3,
		EVENT_GAME_MAP_LEAVE = 4,
		EVENT_GAME_PLAYR_ENTER = 5,
		EVENT_GAME_PLAYR_LEAVE = 6,
		EVENT_GAME_ENTITY_ENTER = 7,
		EVENT_GAME_ENTITY_LEAVE = 8,
		EVENT_ITEM_COUNT_CHANGE = 50,
	};

	LuaContext(App& app);
	~LuaContext();

	// system
	void Initialize();
	void Update();
	void Draw();
	void Exit();
	App& GetApp()const;
	bool NotifyInput(const InputEvent& event);
	lua_State* GetLuaState();

	// script
	static LuaContext& GetLuaContext(lua_State* l);
	static bool DoLuaString(lua_State*l, const string& luaString);
	static bool DoFileIfExists(lua_State*l,const string& name);
	static bool LoadFile(lua_State*l, const string& name);
	static void PushRef(lua_State*l, const LuaRef& luaref);
	static LuaRef CreateRef(lua_State* l);

	bool FindMethod(const string& name);
	bool FindMethod(const string& name, int index);
	bool DoLuaExportFunction(const std::string& funcName, ...);
	bool DoLuaSystemFunctionWithIndex(int systemIndex);
	bool DoLuaSystemFunctionWithIndex(int systemIndex, std::function<int(lua_State*l)>paramFunc);
	void PrintLuaStack(lua_State*l);
	bool HasFileLoaded(const std::string& fileName)const;
	void DoFireLuaSystemEvent(int ent, std::function<int(lua_State*l)>paramFunc);

	// userdata
	static const LuaObjectPtr CheckUserdata(lua_State*l, int index, const string& moduleName);
	static const bool IsUserdata(lua_State*l, int index, const string& name);
	bool IsUserdataHasField(LuaObject& userdata, const std::string& fieldName);
	void CloseUserdatas();
	void NotifyUserdataDestoryed(LuaObject& obj);
	void CallFileWithUserdata(const std::string& name, LuaObject& userdata);
	bool CallFunctionWithUserdata(LuaObject& userdata, const std::string& funcName, std::function<int(lua_State*l)>paramFunc = nullptr);

	// notify userdata
	void NotifyEntityWithMovement(Entity& entity, const std::string& funcName);

	// process, to removed.
	void OnStart();
	void OnUpdate();
	void OnFinish();
	void OnDraw();
	void OnCreated();

	// input event
	bool OnInput(const InputEvent& event);
	bool OnKeyPressed(const InputEvent& event);
	bool OnKeyReleased(const InputEvent& event);
	bool OnMousePressed(const InputEvent& event);
	bool OnMouseReleased(const InputEvent& event);
	bool OnMouseMotion(const InputEvent& event);

	// RegisterModules前加载模块
	void InitUserdataEnv(lua_State* l);
	void RegisterFileData(lua_State* l);
	void RegisterUtils(lua_State* l);

	// modules 注册分先后顺序
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
	void RegisterParticle();
	void RegisterItem();
	// gui register
	void RegisterWindowModule();

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
		game_api_set_start_location,
		game_api_get_life,
		game_api_get_max_life,
		game_api_set_life,
		game_api_set_max_life,
		game_api_get_player,
		// map
		map_api_set_background,
		map_api_create_entity,
		map_api_get_entity_global,
		room_api_get_map,
		// time
		time_api_start,
		time_api_remove,
		time_api_clear,
		// menu
		menu_api_start,
		menu_api_stop,
		// video
		video_api_setFullScreen,
		video_api_isFullScreen,
		video_api_get_wanted_size,
		video_api_get_fps,
		video_api_set_cursor,
		video_api_load_program,
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
		sprite_api_set_color,
		sprite_api_set_outLined,
		sprite_api_set_blinking,
		sprite_api_set_flip_x,
		sprite_api_set_flip_y,
		sprite_api_set_deferred,
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
		// path movement
		movement_path_pai_create,
		movement_path_finding_api_create,
		movement_path_finding_api_set_target,
		// rotate movement
		movement_rotate_api_create,
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
		// particle
		particle_api_create,
		particle_api_play,
		particle_api_stop,
		particle_api_set_preprocess,
		particle_api_draw,
		// item
		item_api_get_item,
		item_api_get_game,
		item_api_set_shadow,
		item_api_set_flow,
		item_api_set_auto_picked,
		item_api_add_count,
		item_api_use_item,
		// weapon
		weapon_api_add,
		weapon_api_drop,
		weapon_api_equip,
		weapon_api_unequip,
		weapon_api_get_entity,
		weapon_api_get_slot,
		weapon_api_get_cur_equip,
		weapon_api_has_weapon,
		weapon_api_get_sprite,
		weapon_api_get_inst,
		// entity
		entity_api_create_sprite,
		entity_api_get_type,
		entity_api_get_map,
		entity_api_get_game,
		entity_api_get_sprite,
		entity_api_try_hurt,
		// entity create
		entity_api_create_title,
		entity_api_create_destimation,
		entity_api_create_dynamic_title,
		entity_api_create_pickable,
		entity_api_create_enemy,
		entity_api_create_bullet,
		entity_api_create_block,
		entity_api_create_chest,
		entity_api_create_trigger,
		// userdata
		userdata_meta_gc,
		userdata_meta_newindex,
		userdata_meta_index,
		userdata_get_utable
		;
		

	// register api
	static void RegisterMetaFunction(lua_State*l, const string &moduleName, const std::string& key, FunctionExportToLua function);
	static void RegisterGlobalFunction(lua_State*l, const std::string& funcName, FunctionExportToLua function);
	static void RegisterFunction(lua_State*l, const string& moduleName, const luaL_Reg* functions);
	static void RegisterType(lua_State*l, const string& moduleName, const luaL_Reg* functions, const luaL_Reg* methods, const luaL_Reg* metamethods);
	static void RegisterType(lua_State*l, const string& moduleName, const string& baseModuleName, const luaL_Reg* functions, const luaL_Reg* methods, const luaL_Reg* metamethods);
	static void AddEnum(lua_State*l, const std::string& enumStr, int value);
	
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
	void EnterPlayer(Player& player);
	void LeavePlayer(Player& player);
	void EnterEntity(Entity& entity);
	void LeaveEntity(Entity& entity);

	// map api
	void RunRoom(MapRoom& room);
	void LeaveRoom(MapRoom& room);
	void RunMap(Map& map);
	void LeaveMap(Map& map);
	void OnMapStart(Map& map);
	void OnMapUpdate(Map& map);
	void OnMapFinish(Map& map);
	void OnMapDraw(Map& map);
	bool OnMapInput(Map& map, const InputEvent& event);

	// time api
	struct TimerData
	{
		LuaRef callBackRef;
		bool repeat;
		bool removed;
		const void* l;
	};
	void AddTimer(const TimerPtr& timer,int contextIndex,const LuaRef& callback, bool repeat);
	void CallTimerRef(const TimerPtr& timer);
	void UpdateTimers();
	void DestoryTimers();
	void RemoveTimer(TimerPtr& timer);

	// item api
	void RunItem(Item& item);
	void OnItemCreated(Item& item);
	bool OnItemObtained(Item& item, ItemAcquired& itemAcquired, Entity& picker);
	bool OnWeaponObtained(Weapon& weapon, ItemAcquired& itemAccquired, Entity& picker);

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
	void DrawDrawables();
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
	bool CreateEntity(const EntityData& entityData, Map& map, LuaRef& initCallback, bool clearStack = false);

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
	static void PushParticle(lua_State*l, ParticleSystem& particle);
	static void PushItem(lua_State*l, Item& item);

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
	static std::shared_ptr<ParticleSystem> CheckParticle(lua_State*l, int index);
	static bool IsParticle(lua_State*l, int index);
	static std::shared_ptr<Item> CheckItem(lua_State*l, int index);
	static bool IsItem(lua_State*l, int index);
	static std::shared_ptr<Entity> CheckEntity(lua_State*l, int index);
	static bool IsEntity(lua_State*l, int index);
	static std::shared_ptr<Player> CheckPlayer(lua_State*l, int index);
	static bool IsPlyaer(lua_State*l, int index);

	// system ref
	static LuaRef mSystemCApiRef;
	static LuaRef mSystemEnumRef;
	static LuaRef mSystemExports;
	static LuaRef mSystemModulesRef;

	// base modules name
	static const string module_main_name;
	static const string module_game_name;
	static const string module_map_name;
	static const string module_room_name;
	static const string module_time_name;
	static const string module_menu_name;
	static const string module_video_name;
	static const string module_font_name;
	static const string module_async_loader_name;
	static const string module_sound_name;
	static const string module_particle_name;
	static const string module_item_name;
	static const string module_weapon_name;
	static const string module_file_data_name;
	static const string module_utils_name;
	// movement modules name
	static const string module_movement_name;
	static const string module_straight_movement_name;
	static const string module_target_movement_name;
	static const string module_path_movement_name;
	static const string module_path_finding_movement_name;
	static const string module_rotate_movement_name;
	// drawable
	static const string module_drawable_name;
	static const string module_sprite_name;
	static const string module_animation_name;
	// entity
	static const string module_entity_name;
	static const string module_player_name;
	static const string module_enemy_name;
	static const string module_entity_bullet_name;
	static const string module_weapon_instance;
	static const string module_block_name;
	static const string module_chest_name;
	static const string module_trigger_name;
	// widget
	static const string module_window_name;


private:
	App& mApp;
	lua_State* l;

	std::unique_ptr<LuaDebug> mLuaDebugger;

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

	std::map<const LuaObject*, std::set<std::string> > mUserdataFields;		/** 保存userdata中作用域中赋值的数据，
																				该数据存储仅用于快速的查找是否存在指定key */
	std::set<std::string> mLoadFileSets;		/* 打开文件的映射，避免重复加载 */
};

#include"lua\luaContext.inl"

#endif