--  物品系统
--  目前大量的物品和装备逻辑写在了lua端

local MapSystem = SystemModule("MapSystem")
local cur_map = nil

local function OnMapEnter(event, scope, custom, map)
	cur_map = map
end

local function OnMapLeave( ... )
	cur_map = nil
end

function MapSystem.Initialize()
	util_log_info("[lua] MapSystem:Init.")

	event_system_register_event(EVENT_GAME_MAP_ENTER, EntitySystem, OnMapEnter)
	event_system_register_event(EVENT_GAME_MAP_LEAVE, EntitySystem, OnMapLeave)
end

function MapSystem.Uninitialize()
	util_log_info("[lua] MapSystem:UnInit.")
end

GlobalExports.map_system_init = MapSystem.Initialize
GlobalExports.map_system_uninit = MapSystem.Uninitialize

