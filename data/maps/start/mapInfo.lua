local Map = ...
local EntitySystem = SystemImport("EntitySystem")
local particle = Particle.Create("meteor")

function Map:OnStarted()
	print("[lua] map started.")
	
	-- init map
	self:SetBackground("sprites/map/start_bg.png")

	-- bg music
	Sound.PlayMusic("level.ogg", true)
	Sound.SetMusicVolume(60)

	-- particle
	particle:Play(400, 300)

	-- start
	dialog_system_start_dialog({
		"start_1", 
		"start_2", 
		"start_3",
		"start_4"})
end

function Map:OnUpdated()

end

function Map:OnDraw()

end

function Map:OnFinished()
	Sound.StopMusic()
	Sound.SetMusicVolume(100)
end
