-- weaponBar 上方状态条-
-- 显示当前武器，和子弹数量

local WeaponBarMT = {
	

}

--------------------------------------------
-- Weapon bar interface
--------------------------------------------

local function OnPlayerEnter(event, scope, custom, player)
	local selfFrame = WeaponBar._selfFrame
	if selfFrame then
		selfFrame:SetVisible(true)
	end
end

local function OnPlayerLeave(event, scope, custom, player)
	local selfFrame = WeaponBar._selfFrame
	if selfFrame then
		selfFrame:SetVisible(false)
	end
end

function WeaponBar.OnLoad()
	local root = Frame.GetRoot()
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

	for k,v in pairs(WeaponBarMT) do 
		weaponFrame[k] = v
	end

	weaponFrame:SetVisible(false)
	WeaponBar._selfFrame = weaponFrame

	event_system_register_event(EVENT_GAME_PLAYR_ENTER, WeaponBar, OnPlayerEnter)
	event_system_register_event(EVENT_GAME_PLAYR_LEAVE, WeaponBar, OnPlayerLeave)
end

function WeaponBar.OnUnLoad()
	
end
