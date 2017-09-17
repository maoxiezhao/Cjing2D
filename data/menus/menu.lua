local tmenu = {};

function tmenu:onStarted()
	print "hello my menu t.";
end

function tmenu:onKeyPressed(key,modifiers)
    local handle = false;
	if (key == "t")then
		handle = true;
		self:closeMenu();
	end
	return handle;
end


function tmenu:closeMenu()
	cjing.Menu.Stop(self);
end

return tmenu;