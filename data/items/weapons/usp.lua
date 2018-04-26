local weapon = ...
-- 武器脚本 
-- Weapon脚本

local WeaponMT = {}
-- 攻击前动作
function WeaponMT:OnWeaponBeforeAttack()
	-- body
end

-- 攻击动作
function WeaponMT:OnWeaponAttack()
	-- body
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

-- 创建时动作
function weapon:OnWeaponCreated()

end

function GetWeaponMT()
	return WeaponMT
end
