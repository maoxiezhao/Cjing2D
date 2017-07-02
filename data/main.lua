-- global setting
package.path = package.path .. ";data/?.lua"

function cjing.Main:onStarted()
	print "Main:onStarted normal."

	-- test menu --
	local t = require("menus/menu");	
	cjing.Menu.Start(self,t);
	t.onFinished = function()
		print "the menu t is overd.";
	end;
end

function cjing.Main:TestTime()
	print "time out!"
	cjing.Timer.Start(self,5000,function()
				self:StartExit()
				end);
end

function cjing.Main:StartExit()
	cjing.Main.Exit();
end

-- notify input --
local gString = "";
function cjing.Main:onKeyPressed(key,modifiers)
    local handle = false;
	gString = gString .. key;
	
	if (key == "f4" and modifiers.ctrl == true)then
		cjing.Main.Exit();
	elseif(key == "f5") then
		cjing.Video.SetFullScreen(not cjing.Video.IsFullScreen());
	end
	
	
	return handle;
end

