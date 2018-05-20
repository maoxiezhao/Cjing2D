-- entity system
-- 负责创建和管理entity
local EntitySystem = SystemModule("EntitySystem")

-----------------------------------------------------
-- Entity event
-----------------------------------------------------
-- local function RegisterEntityEvent(entity, event, callback, custom, scope)
-- 	if not entity then 

-- 		return	
-- 	end
-- end

------------------------------------------------------
-- Entity Metatable
------------------------------------------------------
local EntityMT = {}
function EntityMT:GetUID()
	return self._uid
end

function EntityMT:IsPlayer()
	return self:GetType() == ENTITY_TYPE_PLAYRE
end

function EntityMT:IsEnemy()
	return self:GetType() == ENTITY_TYPE_ENEMEY
end

function EntityMT:OnRemoved()
	EntitySystem.RemoveEntity(self._uid)
end

function EntityMT:SetProperty(prop, value)
	local properties = self._properties or {}
	local property = properties[prop]
	if property ~= value then 
		properties[prop] = value

		entity_prop_fire(self, prop, self)
		self._properties = properties
	end
end

function EntityMT:GetProperty(prop)
	local properties = self._properties or {}
	return properties[prop]
end

local function SetEntityMetatable(entity, meta)
	if not meta or not entity then 
		return 
	end
	for k,v in pairs(meta) do 
		entity[k] = v
	end
end

------------------------------------------------------
-- Entity Manager
------------------------------------------------------
local all_entities = {}
local name_entities = {}

local entity_uid = 1
local cur_game = nil
local cur_map = nil

local function InitEntity(entity)
	if entity then
		for k, v in pairs(EntityMT) do
			entity[k] = v
		end
		entity._uid = entity_uid
		entity_uid = entity_uid + 1
	end
end

function EntitySystem.AddEntity(entity)
	if entity then
		local uid = entity:GetUID()
		all_entities[uid] = entity

		local name = entity:GetName()
		name_entities[name] = entity

		event_system_fire_event(EVENT_GAME_ENTITY_ENTER, entity)
	end
end

function EntitySystem.RemoveEntity(uid)
	if uid then 
		local entity = all_entities[uid]
		if entity then 
			all_entities[uid] = nil
			name_entities[entity:GetName()] = nil

			--entity:RemoveBySelf()
			event_system_fire_event(EVENT_GAME_ENTITY_LEAVE, entity)
			return true
		end
	end
	return false
end

function EntitySystem.GetEntityByUID(uid)
	if uid then 
		return all_entities[uid]
	end
end

function EntitySystem.GetEntityByName(name)
	if name then 
		-- 先从本地缓存获取
		local entity = name_entities[name]
		if entity then 
			return entity
		end
		-- 再从游戏中获取
	end
end

function EntitySystem.GetEntityMT()
	return entitMT
end

------------------------------------------------------
-- Entity Interface
------------------------------------------------------
local function OnGameStart(event, scope, custom, game)
	cur_game = game
end

local function OnGameEnd( ... )
	cur_game = nil
end

local function OnMapEnter(event, scope, custom, map)
	cur_map = map
end

local function OnMapLeave( ... )
	cur_map = nil
end

local function OnPlayerEnter(event, scope, custom, player)
	InitEntity(player)

	local playerTempl = game_content_get_templ("player")
	if playerTempl and playerTempl.metatable then 
		SetEntityMetatable(player, playerTempl.metatable)

		player:InitPlayer()
	end
end

local function OnPlayerLeave(event, scope, custom, player)

end

function EntitySystem.Initialize()
	all_entities = {}
	entity_uid = 1

	event_system_register_event(EVENT_GAME_START,       EntitySystem,  OnGameStart)
	event_system_register_event(EVENT_GAME_END,         EntitySystem,  OnGameEnd)
	event_system_register_event(EVENT_GAME_MAP_ENTER,   EntitySystem,  OnMapEnter)
	event_system_register_event(EVENT_GAME_MAP_LEAVE,   EntitySystem,  OnMapLeave)
	event_system_register_event(EVENT_GAME_PLAYR_ENTER, EntitySystem,  OnPlayerEnter)
	event_system_register_event(EVENT_GAME_PLAYR_LEAVE, EntitySystem,  OnPlayerLeave)
end

function EntitySystem.Uninitialize()
	cur_game = nil
	event_system_unregister_event(EVENT_GAME_START, EntitySystem)
	all_entities = {}
end

----------------------------------------------------------
--  entity factory
----------------------------------------------------------
function EntitySystem.CreateEnemy(name, x, y, layer, direction, templ_name )
	if not cur_map then return end
	print("Create Enemey", name, x, y, layer, direction)
	local enemyData = {
		name = name,
		templ = templ_name,
		x = x,
		y = y,
		layer = layer,
		direction = direction
	}
	local entity = cur_map:Create_enemy(enemyData, InitEntity)
	if entity then 
		EntitySystem.AddEntity(entity)
	end
	return entity
end

local function init_bullet_entity(entity)
	-- init entity base
	InitEntity(entity)

	-- init bullet method
	local bulletTempl = game_content_get_templ("bullet")
	if bulletTempl and bulletTempl.metatable then 
		SetEntityMetatable(entity, bulletTempl.metatable)
	end
end

function EntitySystem.CreateBullet(layer, templ_name)
	if not cur_map then return end
	print("Create Bullet", templ_name, layer)

	local bulletData = {
		templ = templ_name,
		layer = layer,
	}
	local entity = cur_map:Create_bullet(bulletData, init_bullet_entity)
	if entity then 
		EntitySystem.AddEntity(entity)
	end
	return entity
end

GlobalExports.entity_system_init = EntitySystem.Initialize
GlobalExports.entity_system_uninit = EntitySystem.Uninitialize
