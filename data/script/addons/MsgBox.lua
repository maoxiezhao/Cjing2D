-- 模态消息框
-- 消息框目前设置两个类型的
-- 1.ok按钮消息框
-- 2.ok 和 cancel消息框

--------------------------------------------
-- Message box create function
--------------------------------------------
local MsgBoxMT = {
	SetMessageText = function(root, text)
		local label = root._label
		if label then 
			label:SetText(text)
		end
	end,

	SetCallBackOK = function(root, callback)
		local button_ok = root._button_ok
		if button_ok then 
			button_ok:SetCallBack("OnMouseClick", function()
				MsgBox.CloseMsgBox()
				if callback then callback() end
			end)
		end
	end,

	SetCallBackCancel = function(root, callback)
		local button_cancel = root._button_cancel
		if button_cancel then 
			button_cancel:SetCallBack("OnMouseClick", function()
				MsgBox.CloseMsgBox()
				if callback then callback() end
			end)
		end
	end,
}

local function set_msg_box_mt(root)
	if not root then return end
	for k,v in pairs(MsgBoxMT) do
		root[k] = v
	end
end

local function create_msg_box_button(root, name, button_pos)
		-- button
	local buttonData = {
		vertical = "bottom",
		horizontal = "center",
		pos = button_pos, 
		size = {100, 60}, 
	}
	return root:CreateTemplateFrame(name, "Button", buttonData)
end

local function create_msg_box_label(root)
	local labelData = {
		pos  = {0,  45},
		horizontal = "center",
	}
	local label = root:CreateLabel("msgBoxLabel", labelData)
	label:SetFontColor({0,0,0,255})
	label:SetFontSize(32)
	label:SetTextAlign(FONT_ALIGN_CENTER | FONT_ALIGN_TOP)
	label:SetText("Message Box Button!")
	return label
end

local function create_msg_box_ok(root)
	local box_ok = root:CreateFrame("boxOK", {size = {400, 200}})

	box_ok._label =  create_msg_box_label(box_ok)
	box_ok._button_ok =  create_msg_box_button(box_ok, "ok", {0, -10})

	box_ok:SetVisible(false)
	set_msg_box_mt(box_ok)
	return box_ok
end

local function create_msg_box_ok_not(root)
	local box_ok_canel = root:CreateFrame("boxOK_CANEL" ,{size = {400, 200}})

	box_ok_canel._label 	   = create_msg_box_label(box_ok_canel)
	box_ok_canel._button_ok    = create_msg_box_button(box_ok_canel, "ok",    {-80, -10})
	box_ok_canel._button_cancel= create_msg_box_button(box_ok_canel, "cancel",{ 80, -10})

	box_ok_canel:SetVisible(false)
	set_msg_box_mt(box_ok_canel)
	return box_ok_canel
end

--------------------------------------------
-- Message box data parsing
--------------------------------------------

local function msg_box_parse_ok_data(boxFrame, data)
	if not boxFrame or not data then 
		util_log_err("Failed to parse ok message box.")
		return
	end
	boxFrame:SetMessageText(data.text)
	boxFrame:SetCallBackOK(data.ok)
	boxFrame:SetVisible(true)
end

local function msg_box_parse_ok_cancel_data(boxFrame, data)
	if not boxFrame or not data then 
		util_log_err("Failed to parse ok and cancel message box.")
		return
	end
	boxFrame:SetMessageText(data.text)
	boxFrame:SetCallBackOK(data.ok)
	boxFrame:SetCallBackCancel(data.cancel)
	boxFrame:SetVisible(true)
end

--------------------------------------------
-- Message box addon interface
--------------------------------------------
function MsgBox.OnLoad()
	print("MsgBox Load")
	local root = Frame.GetRoot()
	local boxFrame = root:CreateFrame("boxFrame", {size = {800, 600}})
	local maskBg = boxFrame:CreateFrame("MaskBg", {size = {800, 600}})
	maskBg:SetCallBack("OnMouseClick", function() 
		print("Click event cut off by maskbg.") end)

	-- create message box
	local box_back = boxFrame:CreateFrame("boxBack", {
		vertical = "center",
		horizontal = "center",
		pos = {0, -80},
		size = {400, 200},
	})
	local msg_box_ok = create_msg_box_ok(box_back)
	local msg_box_ok_canel = create_msg_box_ok_not(box_back)

	local msg_boxs = {}
	msg_boxs["ok"] 		  = msg_box_ok
	msg_boxs["ok_cancel"] = msg_box_ok_canel

	boxFrame:SetVisible(false)
	MsgBox.msg_boxs = msg_boxs
	MsgBox.selfFrame = boxFrame
end

function MsgBox.OnUnLoad()
	print("Dialog UnLoad")
end

-- BoxData的定义如下
-- local defaultBoxData = {
-- 	  text = "Hello World!",
-- 	  style = "ok"  / "ok_cancel"
-- 	  ok = func,
-- 	  cancel = func,	
-- }
function MsgBox.OpenMsgBox(boxData)
	if not boxData then 
		util_log_err("Open message box without boxData")
		return 
	end

	local boxFrame = MsgBox.selfFrame
	if boxFrame and not boxFrame:GetVisible() then 
		boxFrame:SetVisible(true)

		local style = boxData.style
		if style == "ok" then 
			msg_box_parse_ok_data(MsgBox.msg_boxs[style], boxData)
		elseif style == "ok_cancel" then 
			msg_box_parse_ok_cancel_data(MsgBox.msg_boxs[style], boxData)
		end
	end
end

function MsgBox.CloseMsgBox()
	local boxFrame = MsgBox.selfFrame
	if boxFrame and boxFrame:GetVisible() then 
		local boxes = MsgBox.msg_boxs
		for _, box in pairs(boxes) do
			box:SetVisible(false)
		end

		boxFrame:SetVisible(false)
	end
end

-- Addon Exports
AddonExports.OpenMsgBox = MsgBox.OpenMsgBox