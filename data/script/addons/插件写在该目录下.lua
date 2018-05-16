-- Lua插件系统 0.0.1
-- 所有的插件定义在shared目录addlists.lua中，加载顺序有关。

--------------------------------------------------------
-- Addon 当前特性
--------------------------------------------------------
-- 所有的插件lua，以首字母大小开头。
-- 当插件存在OnLoad、UnLoad、OnUpdate时会自动这三个函数

-- 对于每个addon内部，会默认创建了一个name的表，将函数定义在这个
-- 同名表中，在addon外部也可以通过索引直接访问

-- 对于前面的加载的插件，之后的插件可以通过文件名作为索引访问
-- 定义在AddonExports中的方法
-- 例如： Title在AddonExports.DoFunc = function()end
-- 在之后加载的文件中就可以通过  Title.DoFunc() 访问。

--------------------------------------------------------
-- Frame 当前特性
--------------------------------------------------------

-- CreateFrame 创建一个帧
-- CreateButton 创建一个按钮

-- 创建有先后关系
-- 典型的FrameData如下
local frameData = {
	pos = {300, 100},
	size = {300, 200},
	vertical = "top",
	horizontal = "left",
	grid_pos = {0, 0},
}

--------------------------------------------------------
-- TODO
--------------------------------------------------------



--------------------------------------------------------
-- EXAMPLE
--------------------------------------------------------
function OnLoad()
	print("Title Load")
end

function OnUnLoad()
	print("Title UnLoad")
end

function OnUpdate()
end

function OpenTitle()
	print("Open Title")
end

-- Addon Exports
AddonExports.OpenTitle = OpenTitle


-- 标题界面

function OnLoad()
	print("Title Load")

	local root = Frame.GetRoot()
	local frameData = {
		pos = {300, 100},
		size = {300, 200},
	}
	local titleFrame = root:CreateFrame("t1", frameData)
	local t3 = titleFrame:CreateFrame("t3", {pos = {0,0}, size = {150, 150}})

	t3:SetCallBack("OnMouseClick", function() print("T3 say hello.") end)
	t3:RemoveBySelf()

	local t2 = titleFrame:CreateFrame("t2", {pos = {100,0}, size = {50, 50}})
	local b1 = titleFrame:CreateButton("button1", {pos = {0,0},  size = {150, 150}})

end

function OnUnLoad()
	print("Title UnLoad")
end

function OnUpdate()
	--print("Title Update")
end

function OpenTitle()
	print("Open Title")
end


-- Addon Exports
AddonExports.OpenTitle = OpenTitle

function Title.OnLoad(self)
	print("Title Load")

	local root = Frame.GetRoot()
	local frameData = {
		pos  = {0, 0},
		size = {800, 300},
		vertical = "center",
		horizontal = "center",
	}
	local titleFrame = root:CreateFrame("t1", frameData)
	local titleBg = titleFrame:CreateImage("TitleBg1", {pos = {0,10}, size = {800, 280}})
	if titleBg then 

	end

	local startGame = titleFrame:CreateButton("startGame", { pos = {0,50}, size = {150, 100},horizontal = "center",})
	local aboutGame = titleFrame:CreateButton("aboutGame", { pos = {0,160}, size = {150, 100},horizontal = "center",})
end

function Title.OnUnLoad()
	print("Title UnLoad")
end

function Title.OnUpdate()
	
end

function Title.OpenTitle()
	print("Open Title")
end


-- Addon Exports
AddonExports.OpenTitle = Title.OpenTitle

	--text test
	do 
		-- local label = titleFrame:CreateLabel("lable1", { 
		-- 	text = "你好，挑战者！！！",
		-- 	pos = {0, 150},
		-- 	size = {50, 50},
		-- 	vertical = "center",
		-- 	horizontal = "center"})
		-- label:SetFontColor({255,0,0,255})
		-- label:SetMultline(true)
		-- label:SetFontSize(40)
	end

-- open MessageBox
MsgBox.OpenMsgBox({
  text = "Hello World!",
  style = "ok",
  ok = function() print("Click OK") end,
})