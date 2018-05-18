-- weaponBar 上方状态条-
-- 显示当前武器，和子弹数量

--------------------------------------------
-- Weapon bar interface
--------------------------------------------

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

	local num_label = weaponFrame:CreateLabel("numLabel",{
		 pos = {0, 100},
		 horizontal = "center",
	})
	num_label:SetFontColor({255,255,255,255})
	num_label:SetFontSize(20)
	num_label:SetText("100/100")
	num_label:SetFontFace("ui_pixel")
	num_label:SetTextAlign(FONT_ALIGN_CENTER | FONT_ALIGN_TOP)

	WeaponBar._selfFrame = weaponFrame
end

function WeaponBar.OnUnLoad()
	
end
