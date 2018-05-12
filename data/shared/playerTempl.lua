local PlayerTempl = {}
-- 基本player的配置
-- 目前来看和Game的功能重叠了

PlayerTempl.name = "player"
PlayerTempl.metatable = {
	InitPlayer = function(self)

	end,
}

return PlayerTempl