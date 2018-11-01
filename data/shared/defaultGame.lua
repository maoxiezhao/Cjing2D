-- 默认的游戏存档初始数据
local GameTempl = {}
GameTempl.name = "game"

-- 默认游戏的初始化操作接口
GameTempl.metatable = {
InitDefaultGame = function ( cur_game )
	if not cur_game then return end
	-- default status
	cur_game:SetLife(10)
	cur_game:SetMaxLife(10)
	cur_game:SetValue(GAME_PROP_MAX_SP, 5)
	cur_game:SetStartLocation("minMap", "playerDest")

	return true
end,

-- 当开始游戏后，对默认角色的初始化操作
StartDefaultGame = function ( cur_game )
	if not cur_game then return end
	local player = cur_game:GetPlayer()
	if not player then return end

	-- player statsu 
	--Weapon.AddEquip(player, "usp", true)
	Item.AddItem(cur_game, "usp_bullet", 60)
end,

OnInputKeyDown = function( cur_game, key, modify)
	local player = cur_game:GetPlayer()
	if not player then return end

	-- 换武器？？？
	if key == "1" then 
		player:TrySwapWeaponSlot(0)
	elseif key == "2" then 
		player:TrySwapWeaponSlot(1)
	elseif key == "3" then 
		player:TrySwapWeaponSlot(2)
	elseif key == "r" then 
		player:TrySwapWeaponBullet()
	elseif key == "t" then
		local slot = Weapon.GetCurSlot(player)
		Weapon.DropEquip(player, slot)
	end
end,

OnStopGame = function(game)
	if not game then return end
	game:Stop()

	SetDelayTimer("ResartTitle", 1000, function()
		event_system_fire_event(EVENT_GAME_END)
	end)
end,

}

return GameTempl