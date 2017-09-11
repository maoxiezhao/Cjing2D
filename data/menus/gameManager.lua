local GameManager = {}

-- GameManager
-- 用于管理加载savegame

function GameManager:startGame()
	print("[lua] GameManager.")
	print("[lua] Game Test.")
	
	local testGameName = "save1.data"
	local exists = cjing.Game.exists(testGameName)
	local game = cjing.Game.load(testGameName)
	if not exists then 
		-- default data
		print("[lua] Defualt Game Data.")
		game:setValue("HP", 20)
		game:setValue("SP", 20)
		game:save()
	end 
	
	-- start game
	if game then 
		print("[lua] Start game.")
		self._game = game
		game:start()
	end
end

return GameManager