-- 标题界面

local showTitleTip = true

function Title.OnLoad()
	print("Title Load")

	local root = Frame.GetRoot()
	local frameData = {
		pos  = {0, 0},
		size = {800, 500},
		vertical = "center",
		horizontal = "center",
	}
	local titleFrame = root:CreateFrame("titleFrame", frameData)
	titleFrame:SetCallBack("OnMouseClick", Title.OnPressedAny)
	--title bg
	do
		local titleBg = titleFrame:CreateImage("titleBg", 
			{path = "sprites/menus/title/bg.png", size = {800, 500}})

		-- title ship
		local ship = titleFrame:CreateImage("ship", { 
			path = "sprites/menus/title/titleShip.png",
			pos = {0, 0},
			size = {640, 295},
			vertical = "center",
			horizontal = "center"})
		if ship then 
			local movement = PathMovement.Create("", 12, true)
			if movement then 
				movement:SetPath("2266")
				movement:Start(ship)
			end
		end
		-- title logo
		local title= titleFrame:CreateImage("title", { 
			path = "sprites/menus/title/title.png",
			pos = {0, -200},
			size = {600, 200},
			vertical = "center",
			horizontal = "center"})
	end

	-- title press
	local title_tip = titleFrame:CreateImage("tip", { 
			path = "sprites/menus/title/titleTip.png",
			pos = {0, -70},
			size = {500, 50},
			vertical = "bottom",
			horizontal = "center"})

	-- title button
	local buttons = titleFrame:CreateFrame("buttons", { size = {550, 110}, horizontal = "center",vertical = "bottom",})
	do
		local is_has_game = HasGame()
		local startGame = buttons:CreateTemplateFrame("startGame", "Button", { pos = {0,0},  size = {150, 80}, })
		startGame:SetStateImage("Normal", "sprites/menus/title/button_start_1.png")
		startGame:SetStateImage("Hover",  "sprites/menus/title/button_start_2.png")
		startGame:SetStateImage("Down",   "sprites/menus/title/button_start_3.png")
		startGame:SetClickCallback(Title.OnStartGameClick)
		startGame:SetVisible(is_has_game)

		local continueGame = buttons:CreateTemplateFrame("continueGame", "Button", { pos = {0,0},  size = {150, 80}, })
		continueGame:SetStateImage("Normal", "sprites/menus/title/button_continue_1.png")
		continueGame:SetStateImage("Hover",  "sprites/menus/title/button_continue_2.png")
		continueGame:SetStateImage("Down",   "sprites/menus/title/button_continue_3.png")
		continueGame:SetClickCallback(Title.OnContinueGameClick)
		continueGame:SetVisible(not is_has_game)

		local aboutGame = buttons:CreateTemplateFrame("aboutGame", "Button", { pos = {200,0},  size = {150, 80}, })
		aboutGame:SetStateImage("Normal", "sprites/menus/title/button_about_1.png")
		aboutGame:SetStateImage("Hover",  "sprites/menus/title/button_about_2.png")
		aboutGame:SetStateImage("Down",   "sprites/menus/title/button_about_3.png")
		aboutGame:SetClickCallback(Title.OnOpenAboutGame)

		local exitGame = buttons:CreateTemplateFrame("exitGame", "Button", { pos = {400,0},  size = {150, 80}, })
		exitGame:SetStateImage("Normal", "sprites/menus/title/button_exit_1.png")
		exitGame:SetStateImage("Hover",  "sprites/menus/title/button_exit_2.png")
		exitGame:SetStateImage("Down",   "sprites/menus/title/button_exit_3.png")
		exitGame:SetClickCallback(function() ExitGame() end)
	end
	buttons:SetVisible(false)

	titleFrame:SetVisible(false)
	Title._selfFrame = titleFrame
	Title._buttons = buttons
	Title._tip = title_tip
end

function Title.OnUnLoad()
	print("Title UnLoad")
end

function Title.OnUpdate()
	
end

function Title.OpenTitle()
	local selfFrame = Title._selfFrame
	if selfFrame then 
		selfFrame:SetVisible(true)

		-- 
		Sound.PlayMusic("title_bgm.ogg", true)
	end
end

function Title.CloseTitle()
	self._selfFrame:SetVisible(false)
	Sound.StopMusic()
end

function Title.OnPressedAny(button)
	if showTitleTip then 
		showTitleTip = false
		Title._tip:SetVisible(false)
	end
	Title._buttons:SetVisible(true)
end

-- 开始游戏
function Title.OnStartGameClick(button)
	print("Game Start.")
	local selfFrame = Title._selfFrame
	if selfFrame then 
		selfFrame:SetVisible(false)
	end
	--StartGame()	
	Sound.StopMusic()

	-- msgbox test
	MsgBox.OpenMsgBox({
	  text = "欢迎，是否观看游戏背景介绍",
	  style = "ok_cancel",
	  ok = function() Story.OpenStory() end,
	  cancel = function() StartGame() end,
	})
end

-- 继续游戏
function Title.OnContinueGameClick(button)
	print("Game Start.")
	local selfFrame = Title._selfFrame
	if selfFrame then 
		selfFrame:SetVisible(false)
	end
	--StartGame()	
	Sound.StopMusic()

	-- msgbox test
	StartGame()
end

-- 打开游戏介绍
function Title.OnOpenAboutGame(button)
	print("Game About Mother Funker.")
	DialogBox.OpenDialog({
		"about_1",
		"about_2",
		"about_3",
		"about_4",
	}, 
	function() 
		DialogBox.OpenQuest("about_5", {
			function() DialogBox.OpenDialog({"about_5_1"}) end,
			function() DialogBox.OpenDialog({"about_5_2"}) end,
		})
	end)
end

-- Addon Exports
AddonExports.OpenTitle = Title.OpenTitle