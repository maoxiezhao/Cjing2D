-- 对话框
local DialogSystem = SystemImport("DialogSystem")

local DIALOG_BOX_TYPE_NORMAL = 1
local DIALOG_BOX_TYPE_QUEST = 2
local max_num_quest_button = 3

local cur_box_type = DIALOG_BOX_TYPE_NORMAL
--------------------------------------------
-- Dialog box metatbale
--------------------------------------------

local is_load_content = false
local content_load_speed = 50
local contetn_update_data = {}
local function dialog_box_update_content(data)
	local label = data._label
	local content = data._content
	local index = data._index 
	local len = data._len

	label:SetText(StringUTF8Sub(content, index + 1))
	if index >= len then 
		KillTimer("ConentUpdate")
		is_load_content = false
	end
	data._index = index + 1
end

local function dialog_box_quick_finish()
	if is_load_content then 
		contetn_update_data._index = contetn_update_data._len
	end
end

local function dialog_box_set_context(label, content)
	KillTimer("ConentUpdate")
	local len = utf8.len(content)
	if len > 0 then 
		contetn_update_data._label = label
		contetn_update_data._content = content
		contetn_update_data._index = 1
		contetn_update_data._len = len

		is_load_content = true
		SetTimer("ConentUpdate", content_load_speed, dialog_box_update_content, contetn_update_data)
	end
end

local DialogBoxMT = {
	SetName = function(root, name)
		local name_frame = DialogBox._name 
		if name_frame and name_frame._label then 
			local label = name_frame._label
			if name then
				label:SetText(name)
			end
			name_frame:SetVisible(name ~= nil and name ~= "")
		end
	end,

	SetContent = function(root, content)
		local content_label = DialogBox._content 
		if content_label and content then 
			dialog_box_set_context(content_label, content)
			--content_label:SetText(string.sub(content, 6))
		end
	end,

	SetQuestion = function(root, id, text, callback)
		local quest_frame = DialogBox._quests
		if not quest_frame or not quest_frame._buttons then 
			return 
		end

		local button = quest_frame._buttons[id]
		if button then 
			button:SetVisible(true)
			button:SetLabelText(text)
			button:SetCallBack("OnMouseClick", function()
				DialogBox.OnContinueDialog()
				if callback then callback() end
			end)
		end
	end,
}

local function set_dialog_box_mt(root)
	if not root then return end
	for k,v in pairs(DialogBoxMT) do
		root[k] = v
	end
end

--------------------------------------------
-- Dialog box function
--------------------------------------------

local dialog_ids = {}

local function dialog_box_clear()
	local name_frame = DialogBox._name 
	if name_frame then 
		name_frame:SetVisible(false)
	end

	local quest_frame = DialogBox._quests
	if quest_frame and quest_frame._buttons then 
		local buttons = quest_frame._buttons
		for _, button in ipairs(buttons) do 
			button:SetVisible(false)
		end
	end 

	local content_label = DialogBox._content 
	if content_label then 
		content_label:SetText("")
	end
end

local function dialog_box_parse_id(id)
	print("Open Dialog id:" .. id)
	local dialogData = DialogSystem.OpenDialogByID(id)
	if dialogData then 
		local name = dialogData.name or ""
		local content = dialogData.text or ""
		local dialogFrame = DialogBox.selfFrame
		dialogFrame:SetVisible(true)
		dialogFrame:SetName(name)
		dialogFrame:SetContent(content)

		cur_box_type = DIALOG_BOX_TYPE_NORMAL
	end
end

local function dialog_box_parse_question(id, callbacks)
	print("Open Question id:" .. id)
	local dialogData = DialogSystem.OpenDialogByID(id)
	if dialogData then 
		local name = dialogData.name or ""
		local content = dialogData.text or ""
		local dialogFrame = DialogBox.selfFrame
		dialogFrame:SetVisible(true)
		dialogFrame:SetName(name)
		dialogFrame:SetContent(content)

		local quest = dialogData.quest or {}
		for i, text in ipairs(quest) do
			dialogFrame:SetQuestion(i, text, callbacks[i])
		end
	
		cur_box_type = DIALOG_BOX_TYPE_QUEST
	end
end

function DialogBox.OnContinueDialog()
	dialog_box_clear()

	if #dialog_ids == 0 then 
		DialogBox.selfFrame:SetVisible(false)	
		if DialogBox.OnFinisheCallback then 
			DialogBox.OnFinisheCallback()
		end
	else
		local id = dialog_ids[1]
		table.remove(dialog_ids, 1)

		dialog_box_parse_id(id)
	end
end

function DialogBox.OnClick()
	if cur_box_type == DIALOG_BOX_TYPE_NORMAL then 
		if is_load_content == true then 
			dialog_box_quick_finish()
		else
			DialogBox.OnContinueDialog()
		end
	end
end

--------------------------------------------
-- Dialog box interface
--------------------------------------------

-- 创建对话框名称标签
local function dialog_box_create_name(root)
	local name_frame = root:CreateFrame("box_name", {pos = {40, -30},size = {150, 40}})
	name_frame:CreateImage("boxBg", { 
		path = "sprites/menus/common/msg_box.png",
		size = {150, 40}})

	local labelData = {
		pos  = {0,  5},
		horizontal = "center",
	}
	local name_label = name_frame:CreateLabel("boxLabelName",labelData)
	name_label:SetFontColor({0,0,0,255})
	name_label:SetFontSize(32)
	name_label:SetTextAlign(FONT_ALIGN_CENTER | FONT_ALIGN_TOP)
	name_label:SetText("西门吹雪")

	name_frame:SetVisible(false)
	name_frame._label = name_label
	return name_frame
end

-- 创建对话框内容
local function dialog_box_create_content(root)
	local labelData = {
		pos  = {0,  20},
		size = {600, 120},
		horizontal = "center",
	}
	local name_label = root:CreateLabel("boxLabelName",labelData)
	name_label:SetFontColor({0,0,0,255})
	name_label:SetFontSize(32)
	name_label:SetLineHeight(36)
	name_label:SetMultline(true)

	root._label = name_label
	return name_label
end

-- 创建对话框选项
-- 现在是用按钮在做，实际应该用selection来做
local function dialog_box_create_question(root)
	local quest_buttons = {}
	local quest_frame = root:CreateFrame("questFrame", {
		pos = {0, -10},
		size = {600, 100},
		vertical = "bottom",
		horizontal = "center"
	})

	for i = 1, max_num_quest_button do 
		local button = quest_frame:CreateTemplateFrame("btn" .. i, "Button",{
			pos  = {0, (i - 1) * 33},
			size = {400, 32},
			horizontal = "center"
		})
		button:SetMessageTextProperty({name = "SetPos", param = {0, 0}})
		button:SetStateImage("Hover",  "sprites/menus/common/dialog_button_bg.png")
		button:SetStateImage("Down",   "sprites/menus/common/dialog_button_bg.png")
		button:SetLabelText("Hello")
		button:SetVisible(false)
		quest_buttons[i] = button
	end

	quest_frame._buttons = quest_buttons
	return quest_frame
end

function DialogBox.OnLoad()
	print("Dialog Load")
	local root = Frame.GetRoot()
	local dialogFrame = root:CreateFrame("dialogFrame", {size = {800, 600}})
	local maskBg = dialogFrame:CreateFrame("MaskBg", {size = {800, 600}})
	maskBg:SetCallBack("OnMouseClick", function() 
		print("Click event cut off by maskbg.") end)
	
	-- create dialog box frame
	local box_back = dialogFrame:CreateFrame("boxBack", {
		vertical = "bottom",
		horizontal = "center",
		pos = {0, -50},
		size = {700, 160},
	})
	box_back:SetCallBack("OnMouseClick", DialogBox.OnClick)

	local box_bg = box_back:CreateImage("boxBg", { 
		path = "sprites/menus/common/msg_box.png",
		size = {700, 160}})

	-- create dialog box element
	local box_name =  dialog_box_create_name(box_back)
	local box_content = dialog_box_create_content(box_back)
	local box_quests = dialog_box_create_question(box_back)

	DialogBox._name = box_name
	DialogBox._content = box_content
	DialogBox._quests = box_quests

	set_dialog_box_mt(dialogFrame)
	dialogFrame:SetVisible(false)
	DialogBox.selfFrame = dialogFrame

	event_system_register_event(EVENT_GAME_START_DIALOG, DialogBox, OnEventDialogBoxStart)
end

function DialogBox.OnUnLoad()
	print("Dialog UnLoad")
	KillTimer("ConentUpdate")
end

function OnEventDialogBoxStart(event, scope, custom, ids, callback)
	 DialogBox.OpenDialog(ids)
end

function DialogBox.OpenDialog(dialogIDs, callback)
	if #dialogIDs > 0 then 
		for _, id in ipairs(dialogIDs) do
			table.insert(dialog_ids, id)
		end
		DialogBox.OnContinueDialog()
		DialogBox.OnFinisheCallback = callback
	end
end

function DialogBox.OpenQuest(dialogID, callbacks)
	if dialogID then 
		dialog_box_parse_question(dialogID, callbacks)
		DialogBox.OnFinisheCallback = nil
	end
end

function DialogBox.CloseDialog()
	
end

-- Addon Exports
AddonExports.OpenDialog = DialogBox.OpenDialog
AddonExports.OpenQuest = DialogBox.OpenQuest
AddonExports.CloseDialog = DialogBox.CloseDialog