local Map = ...
local EntitySystem = SystemImport("EntitySystem")

function Map:onStarted()
	print("[lua] map started.")
	
	-- init map
	self:SetBackground("sprites/map/test_bg.png")

	-- test begin
	local enemey = EntitySystem.CreateEnemy(Map, "slime", 128, 160, 0, GAME_DIRECTION_DOWN, "slime")
	-- test end

end

function Map:onUpdated()

end

function Map:onDraw()

end
