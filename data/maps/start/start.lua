local Room = ...

function Room:OnStarted()
	print("[lua] map started.")
end

function Room:OnUpdated()

end

function Room:OnDraw()

end

function Room:OnFinished()

end

function push_box_trigger:OnCanNotifyTrigger(entity)
	if not entity:IsPlayer() then 
		return false
	end
	return true
end

function push_box_trigger:OnNotifyTrigger(entity)
	entity:SetStop()
	dialog_system_start_dialog({
		"start_5", 
		"start_6"},
		function()
			entity:SetNormal()
		end)
end

function open_chest_trigger:OnCanNotifyTrigger(entity)
	if not entity:IsPlayer() then 
		return false
	end
	return true
end

function open_chest_trigger:OnNotifyTrigger(entity)
	entity:SetStop()
	dialog_system_start_dialog({
		"start_7", 
		"start_8"},
		function()
			entity:SetNormal()
		end)
end
