-- Root
-- 在Root中会创建一个Logo界面，同时去异步加载各个资源
-- 之后则创建标题界面
local cur_root = nil
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
	Frame.SetDebug(false)
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

	-- 异步加载的位置不对
	--Frame.LoadFont("ui_normal", "fonts/pingfang.ttf")

	SetDelayTimer("OnFinishAsync", 10, OnFinishAsyncLoading)
end

-- 异步加载结束回调
function OnFinishAsyncLoading()
	-- 打开标题界面
	Title:OpenTitle()
end

