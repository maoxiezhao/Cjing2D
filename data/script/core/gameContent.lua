-- 游戏内容类
-- 该模块负责从shared读取所有的表相关数据
local GameContent = SystemModule("GameContent")

local all_templs = {}
local all_cfgs = {}

local function load_shared_templ(path)
	local full_path = "shared/" .. path
	local templ = SystemDoFile(full_path)
	if not templ then 
		util_log_err("Failed to load game templ:".. full_path)
		return 
	end

	local name = templ.name
	all_templs[name] = templ

	util_log_info("[lua] Load game templ:" .. full_path)
end

function GameContent.Initialize()
	util_log_info("[lua] GameContent:Init.")

	util_log_info("***** Start loading game shared **************")
	-- load templ
	load_shared_templ("defaultGame.lua")
	load_shared_templ("enemyTempl.lua")
	load_shared_templ("playerTempl.lua")
	load_shared_templ("bulletTempl.lua")

	-- load cfgs
	load_shared_templ("addonLists.lua")
	load_shared_templ("dialogsList.lua")

	util_log_info("***** Finish unloading game shared **************")
end

function GameContent.Uninitialize()
	util_log_info("[lua] GameContent:UnInit.")
end

function GameContent.GetSharedTempl(name)
	return all_templs[name]
end

GlobalExports.game_content_init = GameContent.Initialize
GlobalExports.game_content_uninit = GameContent.Uninitialize
GlobalExports.game_content_get_templ = GameContent.GetSharedTempl