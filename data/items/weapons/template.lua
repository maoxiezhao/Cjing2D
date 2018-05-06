-- 武器脚本 
-- Weapon脚本
local Weapon = ...
local cur_game = Weapon:GetGame()

-- local function 
local use_item = Item.UseItem

-- data
local bullet = Item.GetItem(cur_game, "usp_bullet")
local canAttack = true
local swapBullet = false

local WeaponMT = {}
-- 装备的初始化工作
function WeaponMT:OnWeaponInit(cfg)
	self._clip_size = 15
	self._demage = 1

	self:SetAttackDelta(300)
end

-- 攻击前动作
function WeaponMT:OnWeaponBeforeAttack()
	canAttack = true

	-- 检查当前子弹数量
	local clip_size = self._clip_size
	local bullet_count = bullet:GetCount()
	print(bullet_count)
	if bullet_count <= 0 then 
		print("No bullet")
		canAttack = false
		return
	end
end

-- 攻击动作
function WeaponMT:OnWeaponAttack()
	if canAttack then 
		use_item(cur_game, "usp_bullet", 1)
		self:SetAnimation("attack")
	end
end

-- 攻击后动作
function WeaponMT:OnWeaponAfterAttack()
	-- 如果没子弹了需要换弹
end

-- 用于计算武器造成伤害
function WeaponMT:OnWeaponNotifyHurt(player, target)

end

-- 拾取武器时
function WeaponMT:OnWeaponObtained()
	-- body
end

-- 一些额外的武器效果
function WeaponMT:OnWeaponUpdate()
	-- body
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
