-- 开头故事介绍

function Story.OnLoad()
	print("Story Load")
	local root = Frame.GetRoot()
	local story_img = root:CreateImage("storyImg", { 
		path = "sprites/menus/story/s1.png",
		pos = {0, 20},
		size = {700, 350},
		vertical = "top",
		horizontal = "center"})
	story_img:SetVisible(false)

	Story._story_img = story_img
end

function Story.OnUnLoad()
	print("Story UnLoad")
end

function Story.OpenStory()
	print("Story Open!!!")

	Sound.PlayMusic("story.ogg", true)
	DialogBox.OpenDialog({
		"story_1",
	}, 
	Story.Story1)
end


function Story.Story1()
	Story._story_img:SetVisible(true)

	DialogBox.OpenDialog({
		"story_2",
		"story_3",
		"story_4",	
	},
	Story.Story2)
end

function Story.Story2()
	Story._story_img:SetImage("sprites/menus/story/s2.png")

	DialogBox.OpenDialog({
		"story_5",	
		"story_6",
	},
	Story.Story3)
end

function Story.Story3()
	Story._story_img:SetImage("sprites/menus/story/s3.png")

	DialogBox.OpenDialog({
		"story_7",	
		"story_8",
		"story_9",
	},
	Story.Story4)
end

function Story.Story4()
	Story._story_img:SetImage("sprites/menus/story/s4.png")

	DialogBox.OpenDialog({
		"story_10",	
		"story_11",
	},
	Story.Finish)
end

function Story.Finish()
	print("Story Finish.")
	Story._story_img:SetVisible(false)
	Sound.StopMusic()
	StartGame()
end

AddonExports.OpenStory = Story.OpenStory