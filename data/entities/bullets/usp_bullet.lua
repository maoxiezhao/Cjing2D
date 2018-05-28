-- usp bullet 
local Bullet = ...
local particle = Particle.Create("boom")

-- usp bullet
-- usp子弹是一种最基本的子弹

function Bullet:OnCreated()
	local usp_cfg = {
		demage = 1,	
		speed = 500,
		offset = 0.1
	}

	-- custom config --
	self:InitBullet("usp_bullet", usp_cfg)
	self:SetSize({16, 16})
	self:SetOrigin({-8, -8})
	self:SetBoundRotateAnchor(-8, 8)
	self:SetDisappearAnimTime(200)
end

-- 碰到障碍物
function Bullet:OnNotifyObstacleReached()
	-- 碰到障碍物时，播放爆炸动画，同时播放音效
	-- 先清除当前动画
	OnBulletBoom(self)
end

-- 攻击到敌人
local boom = true
function Bullet:OnNotifyAttackEnemy(attack, reaction)
	OnBulletBoom(self)
	if not boom then 
		self:SetEnable(false)
		SetDelayTimer("bullet_boom", 1, OnBulletBoom, self)	
		boom = true
	end
end

function OnBulletBoom(bullet)
	-- 先停止运动
	bullet:StopMovement()
	-- 先清除当前动画
	bullet:ClearSprites()
	-- 默认的爆炸效果
	bullet:Boom("boom_yellow")
	-- 火花效果
	local diff = bullet:GetRotateDiffPos()
	local pos = bullet:GetCameraPos()
	local size = bullet:GetSize()
	local x = pos[1] + size[1] / 2 + diff[1]
	local y = pos[2] + size[2] / 2 + diff[2]
	particle:Play(x, y, 200)
end

-- 计算敌人收到的伤害
function Bullet:OnComputeDemage(enemy)
	-- 默认计算伤害
	self:DemageEnemy(enemy)
	
	-- 同时可以产生一些其他伤害效果
	
end