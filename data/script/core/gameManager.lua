-- 游戏管理类

local GameManager = SystemModule("GameManager")
local cur_game_instance = nil

-- Game Base Calss
local GameBaseClass = {}
GameBaseClass.OnCreate = function()
	local instance = {}
	for k,v in pairs(GameBaseClass) do
		instance[k] = v
	end
	instance._cur_game = nil
	return instance
end

-- 加载游戏存档，如果存档不存在则创建一个新存档
GameBaseClass.LoadSaveGame = function(self, gameName)
	local exists = Game.exists(gameName)
	local game = Game.load(gameName)
	if not game then return false end
	self._cur_game = game
	if not exists then
		-- 如果不存在存档，则需要创建存档
		-- 存档数据应读取
		game:setLife(20)
		game:setMaxLife(20)
		game:setStartLocation("minMap")
		game:save()
	else
		game:setStartLocation("minMap")
	end
	return true
end

GameBaseClass.OnGameStart = function(self )
	local cur_game = self._cur_game
	if not cur_game then
		return 
	end
	cur_game:start()
end

GameBaseClass.GetGameInstance = function(self)
	return self._cur_game
end

function GameManager:StartGame(saveName)
	if not cur_game_instance then
		cur_game_instance = GameBaseClass.OnCreate()
		if not cur_game_instance:LoadSaveGame(saveName) then 
			util_log_err("Faild to load or create save game.")
			return
		end

		event_system_fire_event(EVENT_GAME_START, cur_game_instance)
		cur_game_instance:OnGameStart()
	end
end

-- debug,执行创建一个测试游戏实例（不通过菜单）
function GameManager:StartGameDebug()
	util_log_info("[lua] GameManager:Start Game Debug.")
	local gameName = "save1.data"
	GameManager:StartGame(gameName)
end

GlobalExports.game_manager_start_debug = GameManager.StartGameDebug