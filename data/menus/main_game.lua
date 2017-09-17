local game = ...

-- main game
-- 游戏运行主体
function game:onStarted()
	print("[Lua] Game onStarted.")	
	self._logo = cjing.Sprite.create("sprites/menus/logo.png");
	self._logo:setAnchor(0.5,0.8)
	self._logo:setPos(320, 240)
	self._logoText = nil
	self._debugText = nil
	
	local text = cjing.Text.create({fontName = "arial.ttf"})
	if text then 
		text:setHorizontalAlign("center")
		text:setText("Welcome to Cjing v0.1")
		text:setPos(320, 250)
		text:setLineHeight(25)
		text:setFontColor({16,228,176,255})
		self._logoText = text
	end
	
	local debugText = cjing.Text.create({fontName = "arial.ttf"})
	if debugText then 
		local dubugStr = "FPS:" .. cjing.Video.GetFPS()
		debugText:setText(dubugStr)
		debugText:setPos(20, 440)
		debugText:setLineHeight(25)
		self._debugText = debugText
	end
end

function game:onFinished()
	print("[Lua] Game Finished.")
end

function game:onUpdated()
	--print("[Lua] Game Update.")
end

function game:onDraw()
	if self._debugText then 
		local dubugStr = "FPS:" .. cjing.Video.GetFPS()
		self._debugText:setText(dubugStr)
	end

	-- draw main scene --
	self._logo:draw()
	self._logoText:draw()
	self._debugText:draw()
end 

function game:onMouseMotion(x, y)
	--print(x .."_" .. y)
end

cjing.Main.game = game
game:start()