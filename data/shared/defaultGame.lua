-- 默认的游戏存档初始数据
local GameTempl = {}
GameTempl.name = "game"

-- 默认游戏的初始化操作接口
GameTempl.metatable = {
InitDefaultGame = function ( cur_game )
	if not cur_game then return end
	-- default status
	cur_game:SetLife(20)
	cur_game:SetMaxLife(20)
	cur_game:SetStartLocation("minMap")

	return true
end,

-- 当开始游戏后，对默认角色的初始化操作
StartDefaultGame = function ( cur_game )
	if not cur_game then return end
	local player = cur_game:GetPlayer()
	if not player then return end

	-- player statsu 
	Weapon.AddEquip(player, "usp", true)
	Item.AddItem(cur_game, "usp_bullet", 60)
end,

OnInputKeyDown = function( cur_game, key, modify)
	-- 换武器？？？
	if key == "1" then 
		print("Swap Equip")
	elseif key == "2" then 
		print("Swap Equip")
	elseif key == "3" then 
		print("Swap Equip")
	end


end,

}

return GameTempl