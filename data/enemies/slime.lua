local Enemy = ...

local enemyBaseClass = {
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

}

local slimeCfg = {
	

}

-- 初始化接口，将来会移到其他位置
local function init_enmey(enemy, spriteName)
	if not enemy or not spriteName then 
		utils_api_log_err("The initialized Enemy is nil.")
		return 
	end

	-- 注意表的覆盖
	for k,v in pairs(enemyBaseClass) do 
		enemy[k] = v
	end

	enemy:InitEnemey(slimeCfg)
	enemy:InitSprite(spriteName)
end

function Enemy:OnCreated()
	print("Slime Created")

	init_enmey(self, "slime")
end

function Enemy:OnUpdate()

end

function Enemy:OnKilled()

end
