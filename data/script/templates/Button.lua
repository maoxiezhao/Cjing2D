-- 按钮模板
-- 按钮模板和GUI默认的按钮Widget功能相同，只是基于Frame、Image和Label实现
-- 不同的是默认的Button可以使用默认绘制，而本button必须指定3张image

local function OnMouseEnter(button)
	button:SetState("Hover")
end

local function OnMouseLeave(button)
	button:SetState("Normal")
end

local function OnMouseDown(button)
	button:SetState("Down")
	button:CaptureMouse(true)
end

local function OnMouseUp(button)
	button:SetState("Hover")
end

local function OnMouseCLick(button)
	if button.OnMouseClick then 
		button.OnMouseClick()
	end
end

local ButtonMT = {
	-- 创建按钮的各个控件
	Initialize = function(root, data)
		-- add callback
		root:SetCallBack("OnMouseEnter", OnMouseEnter)
		root:SetCallBack("OnMouseLeave", OnMouseLeave)
		root:SetCallBack("OnMouseDown",  OnMouseDown)
		root:SetCallBack("OnMouseUp",    OnMouseUp)
		root:SetCallBack("OnMouseClick", OnMouseCLick)

		-- clear data pos
		data.pos = nil

		-- button images
		local down_img   = root:CreateImage("down_img",   data)
		local hover_img  = root:CreateImage("hover_img",  data)
		local normal_img = root:CreateImage("normal_img", data)

		local state_images = {}
		state_images["Hover"]  = hover_img
		state_images["Normal"] = normal_img
		state_images["Down"]   = down_img

		for _, img in pairs(state_images) do
			img:SetVisible(false)
		end
		root._state_images = state_images

		-- button label
		local size = data.size
		local labelData = {
			pos = {0, 8},
			horizontal = "center",
			size = {0, size[2] - 8}
		}
		local label = root:CreateLabel("msgBoxLabel", labelData)
		label:SetFontColor({0,0,0,255})
		label:SetFontSize(32)
		label:SetTextAlign(FONT_ALIGN_CENTER | FONT_ALIGN_TOP)
		label:SetText("")
		root._label = label

		root:SetState("Normal")
		return true
	end,	

	SetState = function(root, state)
		local oldImage = root._cur_image
		if oldImage then 
			oldImage:SetVisible(false)
		end

		local stateImages = root._state_images
		local newImage = stateImages[state]
		if newImage then 
			root._cur_image = newImage
			newImage:SetVisible(true)
		end
	end,

	SetStateImage = function(root, state, path)
		local stateImages = root._state_images
		local img = stateImages[state]
		if img then 
			img:SetImage(path)
		end
	end,

	SetClickCallback = function(root, callback)
		root.OnMouseClick = callback
	end,

	SetLabelText = function(root, text)
		local label = root._label
		if label then 
			label:SetText(text)
		end
	end,

	SetMessageTextProperty = function(root, property)
		local label = root._label
		if label then 
			local name = property.name
			local param = property.param
			local func = label[name]
			if func then 
				func(label, param)
			end
		end
	end,
}

local function create_button(parent, name, buttonData)
	local frame = parent:CreateFrame(name, buttonData)
	if not frame then 
		goto ERROR
	end

	-- set button metatable
	for k,v in pairs(ButtonMT) do 
		frame[k] = v
	end

	-- initialze
	if not frame:Initialize(buttonData) then
		goto ERROR
	end

	if frame then
		return frame
	end
::ERROR::
	util_log_err("Failed to get frame root.")
	return nil
end

----------------------------------------------

function OnCreate(parent, name, buttonData)
	return create_button(parent, name, buttonData)
end