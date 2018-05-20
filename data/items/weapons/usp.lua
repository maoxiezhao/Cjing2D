-- 武器脚本 
-- Weapon脚本
local Weapon = ...
local cur_game = Weapon:GetGame()
local EntitySystem = SystemImport("EntitySystem")
local use_item = Item.UseItem

-- 重载部分模板方法
local WeaponOverideMT = {}

-- 攻击动作
local bullet_name = "usp_bullet"
function WeaponOverideMT:OnWeaponAttack()
	local cur_player = self:GetEntity()
	if not cur_player then return end

	if use_item(cur_game, bullet_name, 1) then 
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
end


function GetWeaponMT()
	local weapon_template = game_content_get_templ("weapon")
	if not weapon_template and weapon_template.metatable then 
		util_log_err("The default weapon template is invalid.")
		return 
	end
	local weaponMT = weapon_template.metatable
	for k,v in pairs(WeaponOverideMT) do 
		print(k, v)
		weaponMT[k] = v
	end
	return weaponMT
end

local usp_cfg = {
	_clip_size = 15,	-- 弹夹容量
	_demage = 1,		-- 武器伤害
	_swapSpeed = 2000,	-- 换单速度
	_offset = 5,	    -- 发射偏差值
	_usedSp = 1,		-- 消耗的sp
	_attack_delta = 300,-- 攻击间隔
	_bullet_name = bullet_name
}

-- 创建时动作
function Weapon:OnCreated()
	local weaponMT = GetWeaponMT()
	for k,v in pairs(weaponMT) do 
		self[k] = v
	end

	self:OnWeaponInit(usp_cfg)
end
