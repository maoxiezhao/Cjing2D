-- billboard
-- 本质是不是BIllBoard的实现技术

local board_width  = 50
local board_height = 30

local all_boards = {}
--------------------------------------------
-- Billboard Metatable
--------------------------------------------

--------------------------------------------
-- Billboard Method
--------------------------------------------

local function create_billboard_frame(entity)
	local boardFrame = BillBoard._selfFrame
	if not boardFrame or not entity then return end

	local name = entity:GetName()
	local board = boardFrame:CreateFrame(name, { 
		size = {50, 30}})

	local label = board:CreateLabel("boardLabel",{pos = {5, 10}, horizontal = "center"})
	label:SetFontColor({255,255,255,255})
	label:SetFontSize(20)
	label:SetFontFace("ui_pixel")
	label:SetTextAlign(FONT_ALIGN_CENTER | FONT_ALIGN_TOP)
	label:SetText("APG")
	board._label = label

	board:SetVisible(false)
	return board
end

local function OnUpdateEntityStatus(event, scope, custom, entity)
	if not entity then return end
	local head_label = entity:GetProperty(ENTITY_PROPERTY_HEAD_LABEL)
	if head_label and head_label ~= "" then 
		local board = all_boards[entity]
		if not board then return end

		local label = board._label
		if not label then return end

		label:SetVisible(true)
		label:SetText(head_label)
		board:SetVisible(true)
	else
		local board = all_boards[entity]
		if not board then return end

		board:SetVisible(false)
	end	
end

-- Entity进入map时调用，已经做了筛选
-- 有效的EntityType为 Player、Pickable、Enemy
local function OnEntityEnter(event, scope, custom, entity)
	local board = create_billboard_frame(entity)
	all_boards[entity] = board

	OnUpdateEntityStatus(nil, nil, nil, entity)
	entity_prop_reg(entity, ENTITY_PROPERTY_HEAD_LABEL, BillBoard, OnUpdateEntityStatus)
end

local function OnEntityLeave(event, scope, custom, entity)
	entity_prop_unreg(entity, ENTITY_PROPERTY_HEAD_LABEL, BillBoard)

	local board = all_boards[entity]
	if board then 
		board:RemoveBySelf()
	end
	all_boards[entity] = nil
end

--------------------------------------------
-- Billboard interface
--------------------------------------------
function BillBoard.OnLoad()
	print("BillBoard load")
	local root = Frame.GetRoot()
	local boardFrame = root:CreateFrame("BillboardFrame", {size = {800, 600}})

	event_system_register_event(EVENT_GAME_ENTITY_ENTER,  BillBoard,  OnEntityEnter)
	event_system_register_event(EVENT_GAME_ENTITY_LEAVE,  BillBoard,  OnEntityLeave)

	BillBoard._selfFrame = boardFrame
end

function BillBoard.OnUnLoad()
	print("BillBoard UnLoad")
end

function BillBoard.OnUpdate()
	for entity, board in pairs(all_boards) do
		local pos = entity:GetBillBoardPos()
		if pos[1] >= 0 and pos[1] < 750 and 
		   pos[2] >= 0 and pos[2] < 570 then 

		   local size = entity:GetSize()
		   pos[1] = pos[1] - (board_width - size[1]) / 2
		   pos[2] = pos[2] - board_height
		   board:SetPos(pos)
		end
	end
end

