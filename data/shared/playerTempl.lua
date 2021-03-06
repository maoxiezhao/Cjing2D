local PlayerTempl = {}
PlayerTempl.name = "player"
-- 基本player的配置
-- 目前来看和Game的功能重叠了

local is_recovering = false
local function recover_sp_by_interval(player)
	local cur_sp = player:GetProperty(ENTITY_PROPERTY_SP)
	local max_sp = player:GetProperty(ENTITY_PROPERTY_MAX_SP)
	if cur_sp < max_sp then 
		cur_sp = cur_sp + 1	
		player:SetProperty(ENTITY_PROPERTY_SP, cur_sp)
	end

	if cur_sp == max_sp then 
		player:SetTired(false)
		is_recovering = false
		KillTimer("recover_sp")
	end
end

PlayerTempl.metatable = {
	InitPlayer = function(player)
		local game = player:GetGame()

		-- base property define 
		local hp = game:GetLife()
		local max_hp = game:GetMaxLife()
		player:SetProperty(ENTITY_PROPERTY_HP, hp)
		player:SetProperty(ENTITY_PROPERTY_MAX_HP, max_hp)

		local max_sp = game:GetValue(GAME_PROP_MAX_SP)
		player:SetProperty(ENTITY_PROPERTY_SP, max_sp)
		player:SetProperty(ENTITY_PROPERTY_MAX_SP, max_sp)
		player:SetProperty(ENTITY_PROPERTY_DEMAGE, 5)

		-- custom define 
		player:SetTired(false)
		player._hurt_protect_time = 1500
		player._hurt_protect = false
		-- weapon
		player._cur_slot = Weapon.GetCurSlot(player)
	end,

	SavePlayer = function(player)

	end,

	------------------------------------------------------------------
	--  C++调用的方法
	------------------------------------------------------------------

	-- 每帧调用的刷新函数
	OnUpdate = function(player)
		local cur_sp = player:GetProperty(ENTITY_PROPERTY_SP)
		local max_sp = player:GetProperty(ENTITY_PROPERTY_MAX_SP)

		if cur_sp < max_sp and not is_recovering then 
			is_recovering = true
			SetTimer("recover_sp", 1000, recover_sp_by_interval, player)
		end
	end,

	-- 当碰到block时调用
	OnNotifyCollisionBlock = function(player, block)
		if block then 
			player:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "!!!!")
		end
	end,

	OnNotifyCollisionChest = function(player, chest)
		if chest and not chest:IsOpen() then 
			player:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "!!!!")
		end
	end,

	-- 当facing的entity改变时调用
	OnNotifyFacingEntityChanged = function( player, entity)
		if not entity then 
			player:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "")
		end
	end,

	-- 临时临时临时临时临时处理角色伤害,绝对不能在全部处理伤害！！
	OnNotifyCollisionEnemy = function( player, enemy)
		if enemy and not player._hurt_protect then 
			local sprite = player:GetBodySprite()
			if sprite then
				sprite:SetBlinking(100)
			end

			Sound.PlaySound("hurt.ogg")

			-- hurt lift
			local cur_hp = player:GetProperty(ENTITY_PROPERTY_HP)
			if cur_hp <= 0 then return end

			cur_hp = cur_hp - 1
			player:SetProperty(ENTITY_PROPERTY_HP, cur_hp)

			if cur_hp <= 0 then 
				player:SetStop()
				dialog_system_start_dialog({
					"end_1"},
					function()
						local game = player:GetGame()
						game:OnStopGame()
					end)
				return
			end

			-- attack back
			player:AttackBack()

			-- after hurt
			player._hurt_protect = true
			SetDelayTimer("PlayerHurtProcted", player._hurt_protect_time,
				function ()
					player._hurt_protect = false
					sprite:SetBlinking(0)
				end)
		end
	end,

	------------------------------------------------------------------
	--  非C++调用的自定义方法
	------------------------------------------------------------------
	AttackBack = function(player)
		local movement = StraightMovement.Create()
		if movement then 
			movement:Start(player, function() player:SetNormal() end)

			local degree = player:GetFacingDegree()
			movement:SetSpeed(200)
			movement:SetMaxDistance(32)
			movement:SetAngle(-util_to_radians(degree) + 1.57)
		end
	end,


	-- player sp define
	SetTired = function(player, tired)
		if tired == player._tired then 
			return 
		end

		player._tired = tired

		if tired then 
			is_recovering = true
			SetTimer("recover_sp", 500, recover_sp_by_interval, player)
		end
	end,

	IsTired = function( player )
		return player._tired
	end,

	UseUpSp = function(player, sp)
		local cur_sp = player:GetProperty(ENTITY_PROPERTY_SP)
		cur_sp = cur_sp - sp
		if cur_sp <= 0 then 
			cur_sp = 0
			player:SetTired(true)
		end
		player:SetProperty(ENTITY_PROPERTY_SP, cur_sp)
	end,

	AddHp = function(player, hp)
		local cur_hp = player:GetProperty(ENTITY_PROPERTY_HP)
		cur_hp = cur_hp + sp
		player:SetProperty(ENTITY_PROPERTY_SP, cur_hp)
	end,

	-- Player shifting moving
	OnBeforeShiftMove = function(player)
		local canShift = true
		canShift = not player:IsTired()

		return canShift
	end,

	OnAfterShiftMove = function(player)
		player:UseUpSp(1)
	end,

	-- 装备管理
	TrySwapWeaponBullet = function(player)
		local cur_weapon = Weapon.GetCurWeapon(player)
		if cur_weapon and cur_weapon:IsWeaponGun() and
		 not cur_weapon:IsSwapingBullet() then 
			cur_weapon:SwapBullets(true)
		end
	end,

	TryPickWeaponToSlot = function(player, weaponID, equiped)
		local equiped = Weapon.AddEquip(player, weaponID, equiped)
		if equiped then 
			player._cur_slot = Weapon.GetCurSlot(player)
		end

	end,

	TrySwapWeaponSlot = function(player, slot)
		if player._cur_slot == slot then 
			return 
		end

		if Weapon.HasWeapon(player, slot) and
			Weapon.Equip(player, slot) then
			player._cur_slot = slot
		end
	end,
}

return PlayerTempl