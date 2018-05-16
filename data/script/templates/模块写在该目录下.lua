-- 插件模板 v0.0.1
-- 模板的环境和普通的Addon的环境一样
local addon_env_list = {
	-- uitls log
	["util_log_info"] = "util_log_info",
	["util_log_warn"] = "util_log_warn",
	["util_log_err"]  = "util_log_err",

	-- timer func
	["SetTimer"] 	  = "SetTimer",
	["SetDelayTimer"] = "SetDelayTimer",
	["KillTimer"] 	  = "KillTimer",

	-- some game func
	["StartGame"]	  = "game_manager_start_debug",
	["GetGame"]       = "game_manager_get_game",
	["GetPlayer"]     = "game_manager_get_player",

	-- ui modules
	["PathMovement"]  = "PathMovement",
	["Frame"]	      = "Frame",
}

-- 但是模板不执行OnLoad和OnUnLoad以及OnUpdate
-- 模板必须提供下列的接口用于返回Frame

function OnCreate()
	return frame
end