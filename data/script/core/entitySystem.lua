-- entity system
-- 负责创建和管理entity
local EntitySystem = SystemModule("EntitySystem")

------------------------------------------------------

local EntityMT = {}


function EntityMT:Init(uid)
	self._uid = uid
end

function EntityMT:GetUID()
	return self._uid
end

function EntitySystem.GetEntityMT()
	return entitMT
end

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
		entity:Init(entity_uid)
		entity_uid = entity_uid + 1
	end
end


function EntitySystem.CreateEnemy(map, name, x, y, layer, direction, templ )
	print("Create Enemey", name, x, y, layer, direction)
	local enemyData = {
		name = name,
		templ = templ,
		x = x,
		y = y,
		layer = layer,
		direction = direction
	}
	local entity = map:Create_enemy(enemyData, InitEntity)
	if entity then 
		EntitySystem.AddEntity(entity)
	end
	return entity
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

			entity:RemoveBySelf()
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

local function game_started(game)
	cur_game = game
end

function EntitySystem.Initialize()
	all_entities = {}
	entity_uid = 1
	event_system_register_event(EVENT_GAME_START, EntitySystem, game_started)
end

function EntitySystem.Uninitialize()
	cur_game = nil
	event_system_unregister_event(EVENT_GAME_START, EntitySystem)
	all_entities = {}
end

GlobalExports.entity_system_init = EntitySystem.Initialize
GlobalExports.entity_system_uninit = EntitySystem.Uninitialize
