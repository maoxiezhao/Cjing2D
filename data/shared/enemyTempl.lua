local EnemyTempl = {}
EnemyTempl.name = "enemy"
-- 基本enemey的模板

EnemyTempl.default_cfg = {
	-- atk prop --
	life = 10,
	demage = 1,

	-- base prop
	size = {26, 20},
	origin = {-3, -20},
}

EnemyTempl.metatable =  {
	InitEnemey = function(self, cfg)
		self._life = 5
		self._damage = 2
		self._state = nil
		self:SetSize({26, 20})
		self:SetOrigin({-3, -20})
	end,

	InitSprite = function(self, name)
		local spritePath = "enemies/" .. name
		local sprite = self:CreateSprite(spritePath)
		if sprite then 
		end
	end,

	Update = function(self)
		local state = self._state
		if state then 
			state:Update()
		end
	end,

	SetState = function(self, state)
		if not state then 
			return 
		end
		-- 仅当当前状态不同时，做处理
		local old_state = self._state
		if old_state ~= state then 
			old_state:Leave(self)
			self._state = state
			state:Enter(self)
		end
	end,
}

return EnemyTempl-- enemy data --