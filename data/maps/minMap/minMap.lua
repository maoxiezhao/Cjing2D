local Room = ...
local EntitySystem = SystemImport("EntitySystem")

function Room:OnStarted()
	print("[lua] Room started.")
	-- -- test begin
	local enemey = EntitySystem.CreateEnemy("slime", 128, 160, 0, GAME_DIRECTION_DOWN, "slime")
	-- -- test end

end

function Room:OnUpdated()

end

function Room:OnDraw()

end

function test_trigger:OnCanNotifyTrigger(entity)
	if self:GetType() == ENTITY_TYPE_PLAYRE then 
		return true
	end
	return false
end

function test_trigger:OnNotifyTrigger(entity)
	print("HHHHHHHHHHHH")
end