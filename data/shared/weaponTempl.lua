local WeaponTempl = {}
WeaponTempl.name = "weapon"

WeaponTempl.metatable = {
	
	OnWeaponInit = function (self, cfg )
		self._id		   = cfg._id
		self._clip_size    = cfg._clip_size	  -- 弹夹容量
		self._demage       = cfg._demage		  -- 武器伤害
		self._swapSpeed    = cfg._swapSpeed	  -- 换单速度
		self._offset 	   = cfg._offset	      -- 发射偏差值
		self._usedSp       = cfg._usedSp
		self._bullet_name  = cfg._bullet_name
		self._swapBullet   = false
		self._cur_bullet_count = 0 
		self._first_equiped = true

		local cur_game = self:GetGame()
		self._bullet = Item.GetItem(cur_game, cfg._bullet_name)
		self:SetAttackDelta(cfg._attack_delta)-- 攻击间隔
	end,

	-- 装备该武器
	OnWeaponEquiped = function (self)
		local player = self:GetEntity()
		if player then 
			-- 这里同时可以对player设置一些状态
			entity_event_fire(player, EVENT_ENTITY_EQUIP_WEAPON, self, slot)

			-- 装备上后自动进行非延迟换单
			if self._first_equiped then 
				self:SwapBullets(false)
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

		-- 子彈過少不能攻擊
	--	local bullet = self._bullet
		local bullet_count = self._cur_bullet_count
		if bullet_count <= 0 then 
			print("No bullet")
			return false
		end

		-- 換彈中不能攻擊 
		if self._swapBullet then 
			print("Swaping bullet.")
			canAttack = false
			return false;
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

		if Item.UseItem(cur_game, bullet_name, 1) then 
			self:SetAnimation("attack")
			local cur_layer = cur_player:GetLayer()
			local bullet = EntitySystem.CreateBullet(cur_layer, bullet_name)
			if bullet then 
				-- 设置发射的Enityt和weapon
				bullet:SetFireEntity(cur_player, self)
				-- 发射子弹
				bullet:Firing()
			end
		else
			util_log_warning("Failed to use bullet:" .. bullet_name)
		end
	end,

	-- 
	OnWeaponAfterAttack = function(self)
		-- 处理换单
		local clip_size = self._clip_size
		local bullet = self._bullet
		local bullet_count = bullet:GetCount()
		local cur_count = self._cur_bullet_count
		if bullet_count > 0 and cur_count <= 0 then
			self:SwapBullets(true)
		end

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

	------------------------------------------------------------------
	--  非C++调用的自定义方法
	------------------------------------------------------------------
	-- 是否在换单
	IsSwapingBullet = function ( self )
		return self._swapBullet
	end,

	-- 获取当前子弹
	GetBullet = function(self)
		return self._bullet
	end,

	-- 发射子弹
	FireBullet = function(self, count)
		if count > 0 then 
			self._cur_bullet_count = self._cur_bullet_count - count
			event_system_fire_event(EVENT_WEAPON_FIRE_BULLET, self)
		end
	end,

	-- 获取当前上单数量
	GetCurBulletCount = function( self )
		return self._cur_bullet_count
	end,

	-- 换单操作
	SwapBullets = function(self, is_delay)
		if self._swapBullet then return end

		local bullet = self._bullet
		local clip_size = self._clip_size
		local cur_count = self._cur_bullet_count
		local bullet_count = bullet:GetCount() - cur_count
		local player = self:GetEntity()

		if cur_count < clip_size and player then 
			local delta = clip_size - cur_count
			local real = math.min(delta, bullet_count)
			if real <= 0 then return end

			if is_delay then 
				local delay_time = is_delay and self._swapSpeed or 0
				self._swapBullet = true
				print("Swap Bullet!!!!!!!")
				player:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "Reload!!!")
				SetDelayTimer("SwapBullet", delay_time, function()
					self._cur_bullet_count = cur_count + real
					event_system_fire_event(EVENT_WEAPON_SWAP_BULLET, self)
					self._swapBullet = false
					player:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "")
				end)
			else
				self._cur_bullet_count = cur_count + real
				event_system_fire_event(EVENT_WEAPON_SWAP_BULLET, self)
			end
		end
	end,
}

return WeaponTempl
