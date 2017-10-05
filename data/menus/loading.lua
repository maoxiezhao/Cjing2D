local LoadingScene = {};

-- loading menu
-- 用于加载字体以及其他游戏资源

function LoadingScene:onStarted()
	print "[Lua] LoadingScene Starting.";
	self._asyncLoader = nil
	self._animation = nil
	
	local animation = cjing.Animation.create("menus/loading")
	if animation then 
		animation:setPos(260, 180)
		animation:setSize(120,120)
		animation:start()
		self._animation = animation
	end
	
	local asyncLoader = cjing.AsyncLoader.create()
	if asyncLoader then 
		self._asyncLoader = asyncLoader
		asyncLoader:addTask(self.loadingFunction)
		asyncLoader:setFinishedCallBack(self.loadingFinishedCallBack)
		asyncLoader:run()
	end 
	
end

function LoadingScene:loadingFunction()
	-- 加载默认字体
	cjing.Text.loadDefaultEn()
	
end

function LoadingScene:loadingFinishedCallBack()
	print "[Lua] Loading Finished"
	cjing.Menu.Stop(LoadingScene)
end

function LoadingScene:onDraw()
	if self._animation then 
		self._animation:draw()
	end 
end

return LoadingScene;