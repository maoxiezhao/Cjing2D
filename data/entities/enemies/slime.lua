local Enemy = ...
local Game = game_manager_get_game()

-- enemyBase 提供了sprite的创建
-- 同时创建了一个FSM
local enemyBaseClass = {
	InitEnemey = function(self, cfg)
		self._life = 5
		self._damage = 2
		self._state = nil

		self:SetProperty(ENTITY_PROPERTY_HP, 5)
		self:SetProperty(ENTITY_PROPERTY_DEMAGE, 2)

		self:SetSize({26, 20})
		self:SetOrigin({-3, -20})
	end,

	InitSprite = function(self, name)
		local spritePath = "enemies/" .. name
		local sprite = self:CreateSprite(spritePath)
		if sprite then 
			self._sprite = sprite
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

	SetDirection = function(self, dir)
		local sprite = self._sprite
		if sprite then 
			--print(sprite.SetAnimation)
			sprite:SetDirection(dir)
		end
	end,

	GetSprite = function(self)
		return self._sprite
	end,

	SetLife = function( self, life )
		self._life = life
		self:OnLifeChange()
	end,

	GetLife = function(self)
		return self._life
	end,

	AddLife = function(self, add_life)
		self._life = self._life + add_life
		print("add life:", add_life, "finial lief:", self._life)
		self:OnLifeChange()
	end,

	-- 受击打效果
	AttackBack = function(self, source)
		local movement = StraightMovement.Create()
		if movement then 
			movement:Start(self)

			local degree = source:GetFacingDegree()
			print(degree)
			movement:SetSpeed(200)
			movement:SetMaxDistance(32)
			movement:SetAngle(-util_to_radians(degree))
		end
	end,

	OnLifeChange = function(self)
		self:SetProperty(ENTITY_PROPERTY_HP, self._life)
	end,
}

local slimeCfg = {
	-- atk prop --
	life = 10,
	demage = 1,

	-- base prop
	size = {26, 20},
	origin = {-3, -20},

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

local function test_function()
	print("Call Back")
end

function Enemy:OnCreated()
	print("Slime Created")
	local enemy = self
	init_enmey(enemy, "slime")

	local movement = PathFindingMovement.Create(64)
	if movement then 
		movement:Start(enemy, test_function)
		movement:SetTarget({160, 250})
	end
end

function Enemy:OnUpdate()

end

-- @source 攻击对象
-- @attack 攻击方式
function Enemy:OnHurt(source, attack)
	local life = self:GetLife()
	if life and life > 0 then 
		-- 如果没死，播放被攻击动画
		local sprite = self:GetSprite()
		if sprite then 
			sprite:SetWhiteBlink(true)
			SetDelayTimer("white_sprite", 50, function() 
				sprite:SetWhiteBlink(false) 
			end)	
		end

		-- 播放个后退效果，正常时需要判断
		self:AttackBack(source)
	else
		self:Killed()
	end
end

function Enemy:OnKilled()
	print("DIEDDDDDDDDDDD")
	self:SetEnable(false)
end

function Enemy:OnMovementChanged(movement)
	if movement then 
		local cur_display_direction = movement:GetDirection()
		self:SetDirection(cur_display_direction)
	end
end
