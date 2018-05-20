-- 游戏管理类
local GameManager = SystemModule("GameManager")
local cur_game_instance = nil

--------------------------------------------------------
-- Game Base Metatable
--------------------------------------------------------
local GameBaseClass = {}
GameBaseClass.OnCreate = function()
	local instance = {}
	for k,v in pairs(GameBaseClass) do
		instance[k] = v
	end
	instance._cur_game = nil
	return instance
end

GameBaseClass.IsHasSaveGame = function(gameName)
	return Game.Exists(gameName)
end

-- 加载游戏存档，如果存档不存在则创建一个新存档
GameBaseClass.LoadSaveGame = function(self, gameName)
	local exists = Game.Exists(gameName)
	local game = Game.Load(gameName)
	if not game then return false end
	self._cur_game = game

	-- 读取默认游戏配置
	local default_game = game_content_get_templ("game")
	if not default_game and default_game.metatable then 
		util_log_err("The default game data is invalid.")
		return false
	end

	local gameMT = default_game.metatable
	for k,v in pairs(gameMT) do
		game[k] = v
	end

	-- 如果不存在存档，则需要创建存档
	if not exists then
		if game:InitDefaultGame() then 
			game:SetValue(GAME_PROP_IS_NEW_GAME, true)
			return true
		end
	else
		--game:SetStartLocation("minMap")
	end
	return true
end

GameBaseClass.OnGameStart = function(self )
	local cur_game = self._cur_game
	if not cur_game then
		return 
	end
	cur_game:Start()

	-- 如果是新游戏需要做一次默认初始化
	if cur_game:GetValue(GAME_PROP_IS_NEW_GAME) then 
		cur_game:StartDefaultGame()

		-- 新游戏自动保存
		self:SaveGame()
	end
end

GameBaseClass.GetGameInstance = function(self)
	return self._cur_game
end

GameBaseClass.GetLocalPlayer = function(self)
	local cur_game = self._cur_game 
	if cur_game then 
		return cur_game:GetPlayer()
	end
end

GameBaseClass.SaveGame = function(self )
	local cur_game = self._cur_game 
	if cur_game then 
		if cur_game:GetValue(GAME_PROP_IS_NEW_GAME) then 
			cur_game:SetValue(GAME_PROP_IS_NEW_GAME, false)
		end

		cur_game:Save()
	end 
end

GameBaseClass.OnInputKeyDown = function ( self, key, modify )
	local cur_game = self._cur_game 
	if cur_game then 
		cur_game:OnInputKeyDown(key, modify)
	end
end

--------------------------------------------------------
-- Game Global Interface
--------------------------------------------------------
-- test
local function test_save_game(event, scope, custom, key, modify)
	if key == "y" and cur_game_instance then 
		cur_game_instance:SaveGame()
	end

	if cur_game_instance then 
		cur_game_instance:OnInputKeyDown(key, modify)
	end
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

		-- test
		event_system_register_event(EVENT_INPUT_KEY_DOWN, GameManager, test_save_game)
	end
end

-- debug,执行创建一个测试游戏实例（不通过菜单）
function GameManager:StartGameDebug()
	util_log_info("[lua] GameManager:Start Game Debug.")
	local gameName = "save1.data"
	GameManager:StartGame(gameName)
end

function GameManager.GetGame()
	return cur_game_instance
end

function GameManager.GetPlayer()
	if cur_game_instance then 
		return cur_game_instance:GetPlayer()
	end
end

function GameManager.ExitGame()
	print("Exit Game!!!!")
	Game.Exit()
end

function GameManager.IsHasSaveGame()
	local gameName = "save1.data"
	return GameBaseClass.IsHasSaveGame(gameName)
end

GlobalExports.game_manager_start_debug = GameManager.StartGameDebug
GlobalExports.game_manager_get_game = GameManager.GetGame
GlobalExports.game_manager_get_player = GameManager.GetPlayer
GlobalExports.game_manager_exit_game =GameManager.ExitGame
GlobalExports.game_manager_has_save_game = GameManager.IsHasSaveGame
