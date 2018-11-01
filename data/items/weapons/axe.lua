-- 武器脚本 
-- Weapon脚本
local Weapon = ...
local cur_game = Weapon:GetGame()
local EntitySystem = SystemImport("EntitySystem")
local use_item = Item.UseItem

-- 重载部分模板方法
local WeaponOverideMT = {}

-- 攻击动作
local movement = RotateMovement.Create()
function WeaponOverideMT:OnWeaponAttack()
	local cur_player = self:GetEntity()
	if not cur_player then return end

	self:SetControlEnable(false)

	Sound.PlaySound("hit.ogg")

	local is_flip = self:IsWeaponFlip()
	local rotate_speed = is_flip and -640 or 640
	local rotate_angle = 120

	local instance = self:GetInstance()
	if movement then 
		movement:Start(instance, function() 
			movement:Start(instance, function()self:SetControlEnable(true) end)
			movement:SetSpeed(-rotate_speed)
			movement:SetAngleAmount(rotate_angle)
		end)
		movement:SetSpeed(rotate_speed)
		movement:SetAngleAmount(rotate_angle)
	end
end


function GetWeaponMT()
	local weapon_template = game_content_get_templ("sword")
	if not weapon_template and weapon_template.metatable then 
		util_log_err("The default weapon template is invalid.")
		return 
	end
	local weaponMT = weapon_template.metatable
	for k,v in pairs(WeaponOverideMT) do 
		weaponMT[k] = v
	end
	return weaponMT
end

local usp_cfg = {
	_id = "axe",
	_demage = 0,		-- 武器伤害
	_swapSpeed = 2000,	-- 换单速度
	_offset = 4,	    -- 发射偏差值
	_usedSp = 2,		-- 消耗的sp
	_attack_delta = 600,-- 攻击间隔
	_enable_delta = 100,-- 有效攻击时间
}

-- 创建时动作
function Weapon:OnCreated()
	local weaponMT = GetWeaponMT()
	for k,v in pairs(weaponMT) do 
		self[k] = v
	end

	self:SetAutoPicked(false)
	self:OnWeaponInit(usp_cfg)
	self:SetPosOffset({-10, 0})
	self:SetRotateOffset({10, 0})
end
