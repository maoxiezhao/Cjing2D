-- global setting
package.path = package.path .. ";data/?.lua"

SystemTraceDoFile("script/common.lua")
SystemTraceDoFile("script/core/eventSystem.lua")
SystemTraceDoFile("script/core/gameContent.lua")
SystemTraceDoFile("script/core/gameManager.lua")
SystemTraceDoFile("script/core/addonManager.lua")
SystemTraceDoFile("script/core/entitySystem.lua")

-- 模块初始化
function Main.OnStarted()
	util_log_info("[lua] Main:OnStarted normal.")
	
	collectgarbage('setpause', 200)
	collectgarbage('setstepmul', 200)

	game_content_init()

	entity_system_init()

	-- test
	game_manager_start_debug()	
end

-- 模块更新
function Main.OnUpdate()

end

function Main.OnStop()
	util_log_info("[lua] Main:OnStop normal.")

	entity_system_uninit()

	game_content_uninit()
end

function Main.OnRender()
	
end

SystemExports[CLIENT_LUA_MAIN_START] = Main.OnStarted
SystemExports[CLIENT_LUA_MAIN_UPDATE] = Main.OnUpdate
SystemExports[CLIENT_LUA_MAIN_STOP] = Main.OnStop
SystemExports[CLIENT_LUA_MAIN_RENDER] = Main.OnRender

