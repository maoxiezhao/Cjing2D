-- 武器脚本 
-- Weapon脚本
local Weapon = ...
local cur_game = Weapon:GetGame()
local EntitySystem = SystemImport("EntitySystem")

-- local function 
local use_item = Item.UseItem

-- data
local bullet_name = "usp_bullet"
local bullet = Item.GetItem(cur_game, bullet_name)
local player = nil

-- common prop
local canAttack = true
local swapBullet = false

-- 元表到时候移动到shared中，只在每个weapon中重载特定的方法
local WeaponMT = {}
-- 装备的初始化工作
function WeaponMT:OnWeaponInit(cfg)
	self._clip_size = 15	-- 弹夹容量
	self._demage = 1		-- 武器伤害
	self._swapSpeed = 2000	-- 换单速度
	self._offset = 5		-- 发射偏差值

	self:SetAttackDelta(300)-- 攻击间隔
end

-- 装备该武器
function WeaponMT:OnWeaponEquiped()
	player = self:GetEntity()
	if player then 
		-- 这里同时可以对player设置一些状态
	end
end

-- 卸下该武器
function WeaponMT:OnWeaponUnequiped()
	-- 这里同时可以对player恢复一些状态
	if player then 
		player = nil
	else
		util_log_err("Takeoff weapon without player, weapon ID:")
	end
end

-- 攻击前动作
function WeaponMT:OnWeaponBeforeAttack()
	canAttack = true

	-- 未装备则不可攻击
	if not player then 
		canAttack = false
		return 
	end

	-- 子彈過少不能攻擊
	local bullet_count = bullet:GetCount()
	if bullet_count <= 0 then 
		print("No bullet")
		canAttack = false
		return
	end

	-- 換彈中不能攻擊
	if swapBullet then 
		print("Swaping bullet.")
		canAttack = false
	end
end

-- 攻击动作
function WeaponMT:OnWeaponAttack()
	if canAttack then 
		if use_item(cur_game, bullet_name, 1) then 
			self:SetAnimation("attack")

			local cur_layer = player:GetLayer()
			local bullet = EntitySystem.CreateBullet(cur_layer, bullet_name)
			if bullet then 
				-- 设置发射的Enityt和weapon
				bullet:SetFireEntity(player, self)
				-- 发射子弹
				bullet:Firing()
			end
		else
			util_log_warning("Failed to use bullet:" .. bullet_name)
		end
	end
end

-- 攻击后动作
function WeaponMT:OnWeaponAfterAttack()
	-- 如果没子弹了需要换弹
	if swapBullet then 
		return 
	end

	local clip_size = self._clip_size
	local bullet_count = bullet:GetCount()
	if bullet_count > 0 and (bullet_count % clip_size == 0) then
		print("Swap Bullet")
		swapBullet = true
		SetDelayTimer("SwapBullet", 2000, function() swapBullet = false end)
	end

	print(bullet_count)
end

-- 用于计算武器造成伤害
function WeaponMT:OnWeaponNotifyHurt(player, target)

end

-- 拾取武器时
function WeaponMT:OnWeaponObtained()
	-- 一般的处理方式是拾取武器时判断当前是否存在空的SLot，则先
	-- 放置到空的SLot，如果没有则把当前武器卸下，装备该武器

end

-- 一些额外的武器效果
function WeaponMT:OnWeaponUpdate()
	-- 比如蓄能的武器？？？或者各种奇怪的武器效果
end

function GetWeaponMT()
	return WeaponMT
end

-- 创建时动作
function Weapon:OnCreated()
	local weaponMT = GetWeaponMT()
	for k,v in pairs(WeaponMT) do 
		self[k] = v
	end
	self:OnWeaponInit()
end
