-- hud 上方状态条-
-- 目前显示生命值\气力

local StatusBarMT = {
	Initialize = function(bar)
		bar._max_value = 0
		bar._cur_value = 0
	end,

	SetMaxValue = function(bar, max_value)		
		if max_value ~= bar._max_value then 
			bar._max_value = max_value or 0
			bar:OnValueChange()
		end
	end,

	SetValue = function(bar, value)
		value = value or 0
		value = math.min(value, bar._max_value)
	
		if value ~= bar._cur_value then 
			bar._cur_value = value
			bar:OnValueChange()
		end
	end,

	OnValueChange = function(bar)
		local cur_value = bar._cur_value
		local max_value = bar._max_value

		local progress = bar._progress
		if progress then 
			progress:ResetValue(0, max_value, cur_value)
		end

		local label = bar._num
		if label then 
			label:SetText(cur_value .. "/" .. max_value)
		end
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
		progress_bar:ResetValue(0, 0, 0)
		bar_frame._progress = progress_bar 

		-- set metatable
		for k,v in pairs(StatusBarMT) do
			bar_frame[k] = v
		end
		bar_frame:Initialize()

		return bar_frame
	end
end

local function status_bar_create_hp(root)
	local barData = {
		pos = {0, 0},
		size = {135, 60},
	}
	return status_bar_create_bar(root, "hp", barData)
end

local function status_bar_create_sp(root)
	local barData = {
		pos = {0, 60},
		size = {135, 60},
	}
	return status_bar_create_bar(root, "sp", barData)
end

--------------------------------------------
-- Status bar interface
--------------------------------------------

local function OnPlayerHPChange(event, scope, custom, player)
	local selfFrame = StatusBar._selfFrame
	if selfFrame and player then
		local cur_hp = player:GetProperty(ENTITY_PROPERTY_HP)
		local hp_bar = selfFrame._hp_bar
		hp_bar:SetValue(cur_hp)
	end
end

local function OnPlayerSPChange(event, scope, custom, player)
	local selfFrame = StatusBar._selfFrame
	if selfFrame and player then
		local cur_sp = player:GetProperty(ENTITY_PROPERTY_SP)
		local sp_bar = selfFrame._sp_bar
		hp_bar:SetValue(cur_sp)
	end
end

local function OnPlayerEnter(event, scope, custom, player)
	local selfFrame = StatusBar._selfFrame
	if selfFrame then
		selfFrame:SetVisible(true)
		entity_prop_reg(player, ENTITY_PROPERTY_HP, StatusBar, OnPlayerHPChange)
		entity_prop_reg(player, ENTITY_PROPERTY_SP, StatusBar, OnPlayerSPChange)

		local hp     = player:GetProperty(ENTITY_PROPERTY_HP)
		local max_hp = player:GetProperty(ENTITY_PROPERTY_MAX_HP)
		local sp     = player:GetProperty(ENTITY_PROPERTY_SP)
		local max_sp = player:GetProperty(ENTITY_PROPERTY_MAX_SP)

		local hp_bar = selfFrame._hp_bar
		hp_bar:SetMaxValue(max_hp)
		hp_bar:SetValue(hp)

		local sp_bar = selfFrame._sp_bar
		sp_bar:SetMaxValue(max_sp)
		sp_bar:SetValue(sp)
	end
end

local function OnPlayerLeave(event, scope, custom, player)
	local selfFrame = StatusBar._selfFrame
	if selfFrame then
		selfFrame:SetVisible(false)

		entity_prop_unreg(player, ENTITY_PROPERTY_HP, StatusBar)
		entity_prop_unreg(player, ENTITY_PROPERTY_SP, StatusBar)
	end
end

function StatusBar.OnLoad()
	local root = Frame.GetRoot()
	local statusFrame = root:CreateFrame("StatusFrme", {
		pos = {10, 5},
		size = {135, 120},	
	})

	local hp_bar = status_bar_create_hp(statusFrame)
	local sp_bar = status_bar_create_sp(statusFrame)

	statusFrame._hp_bar = hp_bar
	statusFrame._sp_bar = sp_bar

	statusFrame:SetVisible(false)
	StatusBar._selfFrame = statusFrame

	event_system_register_event(EVENT_GAME_PLAYR_ENTER, StatusBar, OnPlayerEnter)
	event_system_register_event(EVENT_GAME_PLAYR_LEAVE, StatusBar, OnPlayerLeave)
end

function StatusBar.OnUnLoad()
	
end
