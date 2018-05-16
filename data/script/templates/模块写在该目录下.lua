-- 插件模板 v0.0.1
-- 模板的环境和普通的Addon的环境一样
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

-- 在root和addon中是综合实现了创建方法
CreateTemplateFrame(name, teml_name, buttonData)
-- name 	  是创建的名字
-- teml_name  模板名字
-- buttonData 数据

-- 但是模板不执行OnLoad和OnUnLoad以及OnUpdate
-- 模板必须提供下列的接口用于返回Frame
function OnCreate(parent, name, buttonData)
	return create_button(parent, name, buttonData)
end
-- 这里必须要把创建好的Frame返回