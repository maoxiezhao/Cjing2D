-- global setting
package.path = package.path .. ";data/?.lua"

local circlePosX = 200
local circlePosY = 100
local circleRadis = 50
local currStep = 1

function cjing.Main:onStarted()
	print "Main:onStarted normal."

	-- data init --
	self._sprite = {};
	self._animation = nil
	self._text = nil
	
	-- test sprite --
	for index = 1, 64 do
		local sprite = cjing.Sprite.create("sprites/test/duck.png")
		sprite:setSize(50, 50)
		sprite:setPos(((index - 1) % 8)* 70,math.floor(index / 8) * 70)	
		sprite:setRotation(index * 2);
		self._sprite[index] = sprite
	end 
	
	-- test anmiation --
	local animation = cjing.Animation.create("test/explosion")
	if animation then 
		animation:setPos(circlePosX,circlePosY)
		animation:setOpacity(255)
		animation:start()
		self._animation = animation
	end 
	
	-- test menu --
	local menu = require("menus/menu")	
	cjing.Menu.Start(self,menu)
	menu.onFinished = function()
		print "the menu is overd."
	end
	
	-- test time --
	-- cjing.Timer.start(self, 5000, function()  
		-- if self._animation then 
			-- self._animation:setOpacity(0)
		-- end 
	-- end)
	
	-- test movement	
	local targetX = circlePosX + circleRadis * math.cos(math.rad(currStep))
	local targetY = circlePosY + circleRadis * math.sin(math.rad(currStep))
	local movement = cjing.TargetMovement.create()
	if movement then 
		movement:setSpeed(50)
		movement:setTarget(targetX,targetY)
		movement:start(animation,function()  
			--self:TestMovementCallBack()
		end)
	end 
	
	-- test text
	-- local text = cjing.Text.create()
	-- if text then 
		-- self._text = text
	-- end 
	
end

function cjing.Main:TestMovementCallBack()
	print "movement call back."
	currStep = currStep + 10
	local targetX = circlePosX + circleRadis * math.cos(math.rad(currStep))
	local targetY = circlePosY + circleRadis * math.sin(math.rad(currStep))
	local movement = cjing.TargetMovement.create()
	if movement then 
		movement:start(self._animation,function()  
			self:TestMovementCallBack()
		end)
		movement:setSpeed(50)
		movement:setTarget(targetX,targetY)
	end 
end 

function cjing.Main:TestTime()
	print "time out!"
	cjing.Timer.Start(self,5000,function()
				self:StartExit()
				end)
end

function cjing.Main:StartExit()
	cjing.Main.Exit()
end

function cjing.Main:onDraw()
	-- for index = 1, 64 do
		-- if self._sprite[index] then 
			-- self._sprite[index]:draw()
		-- end 
	-- end 
	
	--if self._animation then 
		--self._animation:draw()
	--end 
end 

-- notify input --
local gString = ""
function cjing.Main:onKeyPressed(key,modifiers)
    local handle = false
	gString = gString .. key
	
	if (key == "f4" and modifiers.ctrl == true)then
		cjing.Main.Exit()
	elseif(key == "f5") then
		cjing.Video.SetFullScreen(not cjing.Video.IsFullScreen())
	end
	
	return handle
end

