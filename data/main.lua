-- global setting
package.path = package.path .. ";data/?.lua"

SystemDoFile("script/common.lua")

SystemDoFile("script/core/eventSystem.lua")
SystemDoFile("script/core/entityEvent.lua")
SystemDoFile("script/core/inputSystem.lua")
SystemDoFile("script/core/systemUtils.lua")
SystemDoFile("script/core/addonManager.lua")

SystemDoFile("script/core/gameContent.lua")
SystemDoFile("script/core/gameManager.lua")
SystemDoFile("script/core/entitySystem.lua")
SystemDoFile("script/core/addonManager.lua")

SystemDoFile("script/core/mapSystem.lua")
SystemDoFile("script/core/dialogSystem.lua")

-- 模块初始化
function Main.OnStarted()
	util_log_info("[lua] Main:OnStarted normal.")
	
	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 200)

	math.randomseed(tostring(os.time()):reverse():sub(1, 6)) 
	Video.SetCursor("sprites/menus/cursor.png")

	-- debug setting
	Entity.SetDebugDraw(false)

	-- core system init
	game_content_init()

	entity_system_init()

	addon_manager_init()
	
	-- game module init
	map_system_init()

	dialog_system_init()

	addon_manager_root_start()
	-- test
	--game_manager_start_debug()	
end

-- 模块更新
function Main.OnUpdate()

	addon_manager_update()

end

function Main.OnStop()
	util_log_info("[lua] Main:OnStop normal.")

	-- game module uninit
	addon_manager_uninit()

	dialog_system_uninit()

	map_system_uninit()

	-- core system uninit
	entity_system_uninit()

	game_content_uninit()
end

function Main.OnRender()
	
end

SystemExports[CLIENT_LUA_MAIN_START] = Main.OnStarted
SystemExports[CLIENT_LUA_MAIN_UPDATE] = Main.OnUpdate
SystemExports[CLIENT_LUA_MAIN_STOP] = Main.OnStop
SystemExports[CLIENT_LUA_MAIN_RENDER] = Main.OnRender

