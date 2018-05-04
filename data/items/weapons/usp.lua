-- 武器脚本 
-- Weapon脚本
local Weapon = ...
local game = game_manager_get_game

local WeaponMT = {}
-- 攻击前动作
local canAttack = true
function WeaponMT:OnWeaponBeforeAttack()
	-- body
	-- 检查是否可以攻击
end

-- 攻击动作
function WeaponMT:OnWeaponAttack()
	-- body
	print("Attack")
	if canAttack then 


	end
end

-- 攻击后动作
function WeaponMT:OnWeaponAfterAttack()
	-- body
end

-- 用于计算武器造成伤害
function WeaponMT:OnWeaponNotifyHurt(player, target)

end

-- 拾取武器时
function WeaponMT:OnWeaponObtained()
	-- body
end

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
end
