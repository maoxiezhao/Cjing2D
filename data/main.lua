-- global setting
package.path = package.path .. ";data/?.lua"

function cjing.Main:onStarted()
	print "Main:onStarted normal."

	-- data init --
	self._sprite = {};
	self._animation = nil
	
	-- test sprite --
	for index = 1, 4 do
		local sprite = cjing.Sprite.create("sprites/duck.png")
		sprite:setSize(index * 50,index * 50)
		sprite:setPos((index - 1)* 100,(index - 1) * 80)
		sprite:setRotation(22 * index)
		sprite:setOpacity(index * 50)
		self._sprite[index] = sprite
	end 
	
	-- test anmiation --
	local animation = cjing.Animation.create("explosion")
	if animation then 
		animation:setPos(200,100)
		self._animation = animation
	end 
	
	-- test menu --
	local menu = require("menus/menu")	
	cjing.Menu.Start(self,menu)
	menu.onFinished = function()
		print "the menu is overd."
	end
	
	-- test time --
	cjing.Timer.start(self, 5000, function()  
		if self._animation then 
			self._animation:setOpacity(0)
		end 
	end)
	
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
	for index = 1, 4 do
		if self._sprite[index] then 
			self._sprite[index]:draw()
		end 
	end 
	
	if self._animation then 
		self._animation:draw()
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
		cjing.Video.SetFullScreen(not cjing.Video.IsFullScreen())
	end
	
	return handle
end

