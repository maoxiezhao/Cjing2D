-- 游戏内容类
-- 该模块负责从shared读取所有的表相关数据
local GameContent = SystemModule("GameContent")

function GameContent.Initialize()
	util_log_info("[lua] GameContent:Init.")
end

function GameContent.Uninitialize()
	util_log_info("[lua] GameContent:UnInit.")
end

GlobalExports.game_content_init = GameContent.Initialize
GlobalExports.game_content_uninit = GameContent.Uninitialize