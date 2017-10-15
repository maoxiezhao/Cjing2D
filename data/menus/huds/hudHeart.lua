-- HUD.生命值展示 --
local HudHeart = class("HudHeart")

local gHeartBarWidth = 100
local gDisplayPosX = 20
local gDisplayPosY = 5

local gHeartBarColoar = {
	{34,  177, 60, 255},
	{220, 170, 10, 255},
	{220, 20,  30, 255}
}

-- hud单元的构造函数
function HudHeart:ctor()
	self._game = nil
	self._curLife = 0
	self._maxLife = 0
	self._heartText = nil
	self._heartLabel = nil
	self._heartBottomBar = nil
	self._heartBar = nil
end

-- 初始化hud单元
function HudHeart:initialize(game)
	self._game = game
	
	-- heart text
	local text = cjing.Text.create({fontName = "arial.ttf", fontSize = 14})
	if text then 
		text:setHorizontalAlign("center")
		text:setText("")
		text:setPos(70, 19)
		self._heartText = text
	end
	
	-- heart label
	self._heartLabel = cjing.Sprite.create("sprites/huds/hearts_label.png");
	self._heartLabel:setAnchor(0.5,0.0)
	self._heartLabel:setPos(70, 5)
	
	-- heart bar
	self._heartBottomBar = cjing.Sprite.create({110, 0, 20, 255})
	self._heartBottomBar:setPos(20, 15)
	self._heartBottomBar:setSize(gHeartBarWidth, 18)
	
	self._heartBar = cjing.Sprite.create(gHeartBarColoar[1])
	self._heartBar:setPos(21, 16)
	self._heartBar:setSize(gHeartBarWidth - 2, 16)
	
	self:onCheck()
end

-- 检测当前hud单元状态，判断是否需要刷新
-- 会创建定时器间隔执行onCheck
function HudHeart:onCheck()
	local curLife = self._game:getLife() - 5
	local maxLife = self._game:getMaxLife()
	self._heartText:setText(curLife .. " / " .. maxLife)

	-- change heart bar state
	local heartPer = curLife / maxLife
	local heartBarWidth = heartPer * (gHeartBarWidth - 2)
	self._heartBar:setSize(heartBarWidth, 16)
	if heartPer > 0.6 then
		self._heartBar:setColor(gHeartBarColoar[1])
	elseif heartPer > 0.3 then
		self._heartBar:setColor(gHeartBarColoar[2])
	else 
		self._heartBar:setColor(gHeartBarColoar[3])
	end
	
	cjing.Timer.start(self, 100, function()
		self:onCheck()
	end)
end

-- 绘制hud单元状态
function HudHeart:onDraw()
	-- hart bar
	self._heartBottomBar:draw()
	self._heartBar:draw()
	
	-- heart text
	if self._heartText then 
		self._heartText:draw()
	end 
	
	-- heart label
	self._heartLabel:draw()
end

return HudHeart