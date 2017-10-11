-- global setting
package.path = package.path .. ";data/?.lua"

local circlePosX = 200
local circlePosY = 100
local circleRadis = 50
local currStep = 1

function cjing.Main:onStarted()
	print "[lua] Main:onStarted normal."

	-- data init --
	self._sprite = {};
	self._animation = nil
	self._text = nil
	self._asyncLoader = nil
	self._game = nil
	
	-- set video property
	cjing.Video.setCursor("sprites/menus/cursor.png")
	
	-- loading scene
	local loadingScene = require("menus/loading")	
	if loadingScene then 
		cjing.Menu.Start(self,loadingScene)
		loadingScene.onFinished = function()
			self:FinishedLoading()
		end
	end 
end

function cjing.Main:FinishedLoading()
	print "[Lua] Loading Finished."
	cjing.Sound.playMusic("castle.ogg", true);
	-- 加载存档
	local gameManager = require("menus/gameManager")
	if gameManager then 
		gameManager:startGame()
	end
end

function cjing.Main:StartExit()
	cjing.Main.Exit()
end

function cjing.Main:onDraw()
	-- test text
	if self._text then 
		self._text:draw()
	end 
end 

function cjing.Main:StartGame(game)
	if game then 
		print "[Lua] Starting Game."
		
		local mainGame = cjing.Main.loadFile("menus/main_game")
		if mainGame then 
			mainGame(game)
		end
	end 
end

-- notify input --
local gString = ""
function cjing.Main:onKeyPressed(key,modifiers)
    local handle = false
	gString = gString .. key
	
	if (key == "f4" and modifiers.ctrl == true)then
		cjing.Main.Exit()
	elseif(key == "f5") then
		cjing.Video.setFullScreen(not cjing.Video.isFullScreen())
	end
	
	return handle
end

