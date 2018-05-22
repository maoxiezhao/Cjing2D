-- weaponBar 上方状态条-
-- 显示当前武器，和子弹数量
local max_weapon_slot = 3

local cur_bullet = nil
local cur_weapon = nil

local function set_cur_bullet_count(label, weapon, bullet)
	local cur_count = weapon:GetCurBulletCount()
	local other_count = bullet:GetCount() - cur_count
	print(cur_count, other_count)
	label:SetText(cur_count .. "/" .. other_count)
end

local function set_cur_weapon(frame, weapon)
	if weapon and frame then 
		cur_weapon = weapon

		local weapon_icon = frame._weapon_icon
		local weapon_name = weapon:GetName()
		local icon_path = "sprites/menus/icon/".. weapon_name ..".png"
		weapon_icon:SetImage(icon_path)

		local label = frame._num_label
		local bullet = weapon:GetBullet()
		if bullet then 
			cur_bullet = bullet
			set_cur_bullet_count(label, weapon, bullet)
		else
			label:SetText("∞")
		end
	end
end

local function remove_cur_weapon(frame, weapon)
	if weapon and frame then 
		local weapon_icon = frame._weapon_icon
		weapon_icon:SetImage("")

		local label = frame._num_label
		label:SetText("")
		cur_bullet = nil
		cur_weapon = nil
	end
end

-- 子弹数量变化时
local function OnWeaponBulletChange(event, scope, custom, weapon )
	local selfFrame = WeaponBar._selfFrame
	if selfFrame and cur_bullet and cur_weapon 
		and cur_weapon == weapon then 

		local label = selfFrame._num_label
		set_cur_bullet_count(label, cur_weapon, cur_bullet)
	end
end

--------------------------------------------
-- Weapon bar interface
--------------------------------------------

local function OnPlayerEquipWeapon(event, scope, custom, weapon)
	local selfFrame = WeaponBar._selfFrame
	if selfFrame and weapon then
		set_cur_weapon(selfFrame, weapon, slot)
	end
end

local function OnPlayerUnEquipWeapon(event, scope, custom, weapon)
	local selfFrame = WeaponBar._selfFrame
	if selfFrame and weapon then
		remove_cur_weapon(selfFrame, weapon, slot)
	end
end

local function OnPlayerObtainWeapon(event, scope, custom, weapon, slot)
	local bottomFrame = WeaponBar._selfBottomFrame
	if bottomFrame then 
		slot = slot + 1
		local icons = bottomFrame._icons
		local icon = icons[slot]
		if icon then 
			local weapon_name = weapon:GetName()
			local icon_path = "sprites/menus/icon/".. weapon_name ..".png"
			icon:SetImage(icon_path)
		end
	end
end

local function OnPlayerDropWeapon(event, scope, custom, weapon, slot)
	local bottomFrame = WeaponBar._selfBottomFrame
	if bottomFrame then 
		slot = slot + 1
		local icons = bottomFrame._icons
		local icon = icons[slot]
		if icon then 
			icon:SetImage("")
		end
	end
end

local function OnPlayerEnter(event, scope, custom, player)
	local selfFrame = WeaponBar._selfFrame
	local bottomFrame = WeaponBar._selfBottomFrame
	if selfFrame then
		selfFrame:SetVisible(true)
		bottomFrame:SetVisible(true)

		entity_event_reg(player, EVENT_ENTITY_EQUIP_WEAPON,   WeaponBar, OnPlayerEquipWeapon)
		entity_event_reg(player, EVENT_ENTITY_UNEQUIP_WEAPON, WeaponBar, OnPlayerUnEquipWeapon)
		entity_event_reg(player, EVENT_ENTITY_OBTAIN_WEAPON,  WeaponBar, OnPlayerObtainWeapon)
		entity_event_reg(player, EVENT_ENTITY_DROP_WEAPON,    WeaponBar, OnPlayerDropWeapon)

		local slot = Weapon.GetCurSlot(player)
		local cur_weapon = Weapon.GetCurWeapon(player)
		if cur_weapon and slot then 
			set_cur_weapon(selfFrame, cur_weapon)
			OnPlayerObtainWeapon(nil, nil, nil, cur_weapon, slot)
		end
	end
end

local function OnPlayerLeave(event, scope, custom, player)
	local selfFrame = WeaponBar._selfFrame
	if selfFrame then
		selfFrame:SetVisible(false)
		WeaponBar._selfBottomFrame:SetVisible(false)
	end
end

function WeaponBar.OnLoad()
	local root = Frame.GetRoot()
	-- top weapon bar
	local weaponFrame = root:CreateFrame("WeaponFrame", {
		pos = {-25, 5},
		size = {96, 150},	
		horizontal = "right",
	})
	weaponFrame:CreateImage("barBack", { 
		path =  "sprites/menus/hud/weapon_bar.png",
		pos  = {0, 0},
		size = {96, 96},
	})
	local weapon_icon = weaponFrame:CreateImage("weaponIcon", { 
		path =  "sprites/menus/icon/usp.png",
		pos  = {0, 0},
		size = {96, 96},
	})
	weaponFrame._weapon_icon = weapon_icon

	local num_label = weaponFrame:CreateLabel("numLabel",{
		 pos = {0, 100},
		 horizontal = "center",
	})
	num_label:SetFontColor({255,255,255,255})
	num_label:SetFontSize(20)
	num_label:SetText("100/100")
	num_label:SetFontFace("ui_pixel")
	num_label:SetTextAlign(FONT_ALIGN_CENTER | FONT_ALIGN_TOP)
	weaponFrame._num_label = num_label

	weaponFrame:SetVisible(false)
	WeaponBar._selfFrame = weaponFrame

	-- bottom weapon bar
	local bottomWeaponFrame = root:CreateFrame("BottomFrame", {
		pos = {0, 5},
		size = {210, 80},	
		vertical = "bottom",
		horizontal = "right",
	})
	local icons = {}
	for i = 1, max_weapon_slot do 
		bottomWeaponFrame:CreateImage("barBack" .. i, { 
			path =  "sprites/menus/hud/weapon_bar.png",
			pos  = {(i - 1) * 70, 0},
			size = {60, 60},
		})

		local weapon_icon = bottomWeaponFrame:CreateImage("weaponIcon" .. i, { 
			path =  "",
			pos  = {(i - 1) * 70, 0},
			size = {60, 60},
		})
		icons[i] = weapon_icon
		local label = bottomWeaponFrame:CreateLabel("label" .. i,{
			 pos  = {(i - 1) * 70, 0},
			 size = {60, 60},
		})
		label:SetFontColor({255,255,255,255})
		label:SetFontSize(20)
		label:SetText(" " .. tostring(i))
		label:SetFontFace("ui_pixel")
	end
	bottomWeaponFrame._icons = icons
	bottomWeaponFrame:SetVisible(false)
	WeaponBar._selfBottomFrame = bottomWeaponFrame

	event_system_register_event(EVENT_GAME_PLAYR_ENTER,    WeaponBar,  OnPlayerEnter)
	event_system_register_event(EVENT_GAME_PLAYR_LEAVE,    WeaponBar,  OnPlayerLeave)
	event_system_register_event(EVENT_WEAPON_FIRE_BULLET,  WeaponBar,  OnWeaponBulletChange)
	event_system_register_event(EVENT_WEAPON_SWAP_BULLET,  WeaponBar,  OnWeaponBulletChange)
end

function WeaponBar.OnUnLoad()
	
end
