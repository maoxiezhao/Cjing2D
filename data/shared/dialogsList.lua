local DialogsList = {}
DialogsList.name = "DialogsList"
DialogsList.dialogs = {}

local function AddDialog(id, data)
	DialogsList.dialogs[id] = data
end

--------------------------------------------------------
-- ABOUT GAMG
--------------------------------------------------------
AddDialog(
	"about_1",{
	name = "ZZZY",
	text = [[
	欢迎游玩Lost Universo, 这是游戏介绍
]],
})

AddDialog(
	"about_2",{
	name = "ZZZY",
	text = [[
	同时也用这个介绍来测试游戏对话框和对话列表
]],
})

AddDialog(
	"about_3",{
	name = "ZZZY",
	text = [[
	本游戏基于Cjing v0.1.0实现
	Cjing v0.1.0 是Cjing系列的2D版本，使用C++和Lua基于Opengl实现
]],
})

AddDialog(
	"about_4",{
	name = "ZZZY",
	text = [[
	系统实现2D游戏相关的功能，并在Lua中提供了接口，提供了Sprite、Movement、Particle、GUI、Shader、Entity、Font等基本功能的支持
]],
})

AddDialog(
	"about_5",{
	name = "Admin",
	text = [[
	下面做个对话分支功能测试，喜欢吃葡萄么？
]],
	quest = {
	"喜欢",
	"不喜欢"}
})

AddDialog(
	"about_5_1",{
	name = "ZZZY",
	text = [[
	吃葡萄不吐葡萄皮，不吃葡萄不吐葡萄皮
]],
})

AddDialog(
	"about_5_2",{
	name = "ZZZY",
	text = [[
	不喜欢就算咯，分支测试结束
]],
})



return DialogsList