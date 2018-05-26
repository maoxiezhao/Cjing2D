-- Root
-- 在Root中会创建一个Logo界面，同时去异步加载各个资源
-- 之后则创建标题界面
local cur_root = nil
local cur_logo = nil

local FrameMT = {
	CreateTemplateFrame = function(parent, name, templ_name, ...)
		return CreateTemplate(parent, name, templ_name, ...)
	end,
}

function OnLoad()
	local root = Frame.GetRoot()
	if not root then 
		util_log_err("Failed to get frame root.")
		return
	end
	cur_root = root

	-- debug
	Frame.AddCustom("CreateTemplateFrame", FrameMT.CreateTemplateFrame)
	Frame.SetDebug(false)		-- 显示Widget Debug 
	Frame.DrawDebugBoard(false)	-- 显示调试面板
end

function OnUnLoad()
	-- 卸载时一定要清除回调
	if cur_root	 then 
		cur_root:ClearCallBacks()
	end
end

-- 界面入口
function OnRootStart()
	print("Addon main start.")

	cur_logo = cur_root:CreateImage("TeamLogo", { 
		path = "sprites/menus/logo.png",
		size = {400, 200},
		vertical = "center",
		horizontal = "center"})
	cur_logo:SetVisible(true)

	-- 首先处理异步加载
	Frame.LoadFont("ui_pixel", "fonts/Mouse.ttf", true)


	-- 暂时未实现延迟加载，模拟一个加载时间
	SetDelayTimer("OnFinishAsync", 1000, function()
		OnFinishAsyncLoading()
	end)
end

-- 异步加载结束回调
function OnFinishAsyncLoading()
	cur_logo:SetVisible(false)

	SetDelayTimer("OnFinishAsync", 500, function()
		Title:OpenTitle()
	end)

	
end

