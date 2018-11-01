local SwordTempl = {}
SwordTempl.name = "sword"

SwordTempl.metatable = {
	
	OnWeaponInit = function (self, cfg )
		self._id		   = cfg._id
		self._demage       = cfg._demage		  -- 武器伤害
		self._swapSpeed    = cfg._swapSpeed	 	  -- 换单速度
		self._offset 	   = cfg._offset	      -- 发射偏差值
		self._usedSp       = cfg._usedSp		  
		self._first_equiped = true

		self:SetAttackEnableDelta(cfg._enable_delta) 
		self:SetAttackDelta(cfg._attack_delta)    -- 攻击间隔
		self:SetNotifyCollision(true)
	end,

	-- 装备该武器
	OnWeaponEquiped = function (self)
		local player = self:GetEntity()
		if player then 
			-- 这里同时可以对player设置一些状态
			entity_event_fire(player, EVENT_ENTITY_EQUIP_WEAPON, self, slot)

			-- 装备上后自动进行非延迟换单
			if self._first_equiped then 
				self._first_equiped = false
			end
		end
	end,

	-- 卸下该武器
	OnWeaponUnequiped = function (self )
		local player = self:GetEntity()
		if player then 
			-- 这里同时可以对player设置一些状态
			entity_event_fire(player, EVENT_ENTITY_UNEQUIP_WEAPON, self, slot)
		end
	end,

	-- 攻击前判定
	OnWeaponBeforeAttack = function (self)
		local player = self:GetEntity()

		-- 未装备则不可攻击
		if not player then 
			print("No Equiped.")
			return false
		end

		if player:IsTired() then 
			print("Tired!!")
			canAttack = false
			return false;
		end

		return true
	end,

	-- 攻击时效果
	OnWeaponAttack = function(self)
		local cur_player = self:GetEntity()
		if not cur_player then return end

		local cur_game = self:GetGame()
		if not cur_game then return end

		-- 直接播放攻击效果
		-- 同时设置player State
		self:SetAnimation("attack")

	
	end,

	-- 攻击后处理
	OnWeaponAfterAttack = function(self)

		-- 处理气绝
		local player = self:GetEntity()
		local usedSp = self._usedSp or 0
		player:UseUpSp(usedSp)
	end,

	-- 用于计算武器造成伤害, 一般仅用于近战武器
	OnWeaponNotifyHurt = function(self, player, target)

	end,

	-- 拾取武器时
	OnWeaponObtained = function(self, picker)
		-- 一般的处理方式是拾取武器时判断当前是否存在空的SLot，则先
		-- 放置到空的SLot，如果没有则把当前武器卸下，装备该武器
		if picker and picker.TryPickWeaponToSlot then 
			picker:TryPickWeaponToSlot(self._id, true)

			local slot = Weapon.GetCurSlot(picker)
			entity_event_fire(picker, EVENT_ENTITY_OBTAIN_WEAPON, self, slot)
		end
	end,
	
	-- 一些额外的武器效果
	OnWeaponUpdate = function(self)
		-- 比如蓄能的武器？？？或者各种奇怪的武器效果
	end,

	-- 武器被扔下，一般用于创建pickable
	OnWeaponDroped = function(self)
		local cur_player = self:GetEntity()
		if not cur_player then return end

		local cur_map = cur_player:GetMap()
		if not cur_map then return end 

		local slot = Weapon.GetCurSlot(cur_player)
		entity_event_fire(cur_player, EVENT_ENTITY_DROP_WEAPON, self, slot)

		local pos = cur_player:GetCenterPos()
		local data  = {
		  x = pos[1],
		  y = pos[2],
		  layer = cur_player:GetLayer(),
		  itemName = self._id,
		  itemCount = 1,
		}

		local EntitySystem = SystemImport("EntitySystem")
	 	EntitySystem.CreatePickable(data)
	end,

	OnLoseFocus = function(self, pickable)
		if pickable then 
			pickable:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "")
		end
	end,

	OnGainFocus = function(self, pickable)
		if pickable then 
			pickable:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, self._id)
		end
	end,

	OnTryAttackEnemey = function(self, enemy)
		local cur_player = self:GetEntity()
		if not cur_player then return end

		if enemy:IsHurting() then 
			return 
		end

		local instance = self:GetInstance()
		enemy:TryHurt(ATTACK_COMBAT, instance)	
	end,

	OnComputeDemage = function(self, entity)
		local demage = self._demage
		if demage and entity then 
			entity:AddLife(-demage)
		end
	end,

	------------------------------------------------------------------
	--  非C++调用的自定义方法
	------------------------------------------------------------------
	GetWeaponID = function( self)
		return self._id
	end,

	IsWeaponGun = function( self )
		return false
	end,
}


return SwordTempl