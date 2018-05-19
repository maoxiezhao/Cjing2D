-- hud 上方状态条-
-- 目前显示生命值\气力

local StatusBarMT = {
	SetValue = function(bar, value)

	end,
}

-- Status bar property define
local icon_size = {27, 27}

local function status_bar_create_bar(root, name, data)
	if not root then return end
	local bar_frame = root:CreateFrame("BarFrame" .. name, data)
	if bar_frame then 
		-- status icon
		local icon_path = "sprites/menus/hud/heart_" .. name .. ".png"
		bar_frame:CreateImage("icon", { 
			path = icon_path,
			pos = {0, 0},
			size = {27, 27},
		})

		-- status label
		local status_label = bar_frame:CreateLabel("boxLabelName",{ pos = {40, 5}})
		status_label:SetFontColor({255,255,255,255})
		status_label:SetFontSize(20)
		status_label:SetText("10/10")
		status_label:SetFontFace("ui_pixel")
		bar_frame._num = status_label

		-- progress bar.using img now.
		local progress_img_path = "sprites/menus/hud/point_" .. name .. ".png"
		local progress_bar = bar_frame:CreateTemplateFrame("progress", "ProgressBar",{
			pos = {0, 27},
			size = {134, 28},
		})
		progress_bar:SetBorderImage("sprites/menus/hud/heart_bar.png")
		progress_bar:SetProgressImage(progress_img_path, {6, 6}, {122, 17})
		progress_bar:ResetValue(0, 10, 10)
		bar_frame._progress = progress_bar 

		-- set metatable
		for k,v in pairs(StatusBarMT) do
			bar_frame[k] = v
		end
		return bar_frame
	end
end

local function status_bar_create_hp(root)
	local barData = {
		pos = {0, 0},
		size = {135, 60},
	}
	status_bar_create_bar(root, "hp", barData)
end

local function status_bar_create_sp(root)
	local barData = {
		pos = {0, 60},
		size = {135, 60},
	}
	status_bar_create_bar(root, "sp", barData)
end

--------------------------------------------
-- Status bar interface
--------------------------------------------

function StatusBar.OnLoad()
	local root = Frame.GetRoot()
	local statusFrame = root:CreateFrame("StatusFrme", {
		pos = {10, 5},
		size = {135, 120},	
	})

	local hp_bar = status_bar_create_hp(statusFrame)
	local sp_bar = status_bar_create_sp(statusFrame)

	StatusBar._selfFrame = statusFrame
end

function StatusBar.OnUnLoad()
	
end
