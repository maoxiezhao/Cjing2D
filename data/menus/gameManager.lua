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
		game:setLife(20)
		game:setMaxLife(20)
		game:setStartLocation("minMap")
		game:save()
	end 
	-- test map
	game:setLife(3)
	game:setStartLocation("minMap")
	
	-- start game
	if game then 
		cjing.Main:StartGame(game)
	end
end



return GameManager