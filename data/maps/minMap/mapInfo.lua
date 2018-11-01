local Map = ...
local EntitySystem = SystemImport("EntitySystem")

function Map:OnStarted()
	print("[lua] map started.")
	
	-- init map
	self:SetBackground("sprites/map/test_bg.png")
end

function Map:OnUpdated()

end

function Map:OnDraw()

end

