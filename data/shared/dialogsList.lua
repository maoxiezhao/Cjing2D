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

--------------------------------------------------------
--  GAMG STORY
--------------------------------------------------------

AddDialog(
	"story_1",{
	name = "",
	text = [[
	故事发生在2500年...
]],
})
AddDialog(
	"story_2",{
	name = "",
	text = [[
	地球资源匮乏，为了抢夺少数的资源，战争不断，城市崩塌，人类社会即将毁灭
]],
})
AddDialog(
	"story_3",{
	name = "",
	text = [[
	此时，人类偶然收到距今5000万光年外的一颗星球的光，人类发现这颗星球上蕴含着大量的高能源矿物，这被视为人类的希望之光。这颗星球命名为“艾尔斯特拉”
]],
})
AddDialog(
	"story_4",{
	name = "",
	text = [[
	人类暂时放下战争，集全球之力，创造了大型空间跃迁引擎，并以此创建了一艘巨大的行星挖掘舰——石心号
]],
})
AddDialog(
	"story_5",{
	name = "",
	text = [[
	作为全球之力的结晶，石心号拥有完整的自循环系统，可容纳科研、技术、安保人员2000人，并装备有引力裂解设备，用来裂解挖掘星球
]],
})
AddDialog(
	"story_6",{
	name = "",
	text = [[
	带着全部地球人的希望，石心号使用空间跃迁技术，前往到达距地球5000万光年的“艾尔斯特拉”星
]],
})
AddDialog(
	"story_7",{
	name = "",
	text = [[
	石心号在“艾尔斯特拉”的采集任务顺利进行着，同时在这个星球上发现了一个非自然产生的石头，科研人员将其命名为信标，并带回了石心号
]],
})
AddDialog(
	"story_8",{
	name = "",
	text = [[
	在石心号，科研人员对信标进行的分析，发现了非已知的特殊成分，但却无法获得更多的信息。与此同时，石心号完成的了采集任务，准备返回地球
]],
})
AddDialog(
	"story_9",{
	name = "",
	text = [[
	石心号的人们欢呼于这次大丰收，在全员进入到冬眠仓后，石心号开启了空间跃迁
]],
})
AddDialog(
	"story_10",{
	name = "",
	text = [[
	穿梭在亚空间中，石心号所有人都已经冬眠，只能听到冰冷的仪器声，而放置在实验仓库的信标，此时却开始运作起来
]],
})
AddDialog(
	"story_11",{
	name = "",
	text = [[
	而你，作为石心号安保的一员，却在这时候醒来...
]],
})



return DialogsList