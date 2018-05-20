local PlayerTempl = {}
-- 基本player的配置
-- 目前来看和Game的功能重叠了

PlayerTempl.name = "player"
PlayerTempl.metatable = {
	InitPlayer = function(player)
		local game = player:GetGame()

		local hp = game:GetLife()
		local max_hp = game:GetMaxLife()
		player:SetProperty(ENTITY_PROPERTY_HP, hp)
		player:SetProperty(ENTITY_PROPERTY_MAX_HP, max_hp)

		local max_sp = game:GetValue(GAME_PROP_MAX_SP)
		player:SetProperty(ENTITY_PROPERTY_SP, max_sp)
		player:SetProperty(ENTITY_PROPERTY_MAX_SP, max_sp)

		player:SetProperty(ENTITY_PROPERTY_DEMAGE, 5)
	end,

	SavePlayer = function(player)

	end,
}

return PlayerTempl