-- HUD.生命值展示 --
local HudHeart = class("HudHeart")

function HudHeart:ctor()
	self._game = nil
end

function HudHeart:initialize(game)
	self._game = game
end

function HudHeart:onUpdate()

end

function HudHeart:onDraw()

end

return HudHeart