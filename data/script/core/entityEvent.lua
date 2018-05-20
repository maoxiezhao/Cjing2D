local EntityEvent = SystemModule("EntityEvent")
	
local _xpcall = xpcall
local _debug_traceback = debug.traceback

-----------------------------------------------------------

local function init_entity_event_set(utable)
	if not utable.objects then
		utable.objects = {}
	end
end

local function entity_event_register(entity, event, scope, callback)
	local entity_event_set = entity:GetUTable()
	if not entity_event_set then 
		util_log_err("Failed to register entity event, the entity have not utable.", event)
		return 
	end

	init_entity_event_set(entity_event_set)
	return event_set_reg(entity_event_set, event, scope, callback) 
end

local function entity_event_unregister(entity, event, scope)
	local entity_event_set = entity:GetUTable()
	if not entity_event_set then 
		util_log_err("Failed to unregister entity event, the entity have not utable.", event)
		return 
	end

	init_entity_event_set(entity_event_set)
	return event_set_unreg(entity_event_set, event, scope) 
end

local function entity_event_fire(entity, event, ...)
	local entity_event_set = entity:GetUTable()
	if not entity_event_set then 
		util_log_err("Failed to fire entity event, the entity have not utable.", event)
		return 
	end
	init_entity_event_set(entity_event_set)
	event_set_fire(entity_event_set, event, ...)
end

local function entity_prop_register(entity, prop, scope, callback, custom)
	local ev = EVENT_ENTITY_PROPERTY_CHANGE + prop
	entity_event_register(entity, ev, scope, callback)
end

local function entity_prop_unregister(entity, prop, scope)
	local ev = EVENT_ENTITY_PROPERTY_CHANGE + prop
	entity_event_unregister(entity, ev, scope)
end

local function entity_prop_fire(entity, prop, ...)
	local ev = EVENT_ENTITY_PROPERTY_CHANGE + prop
	entity_event_fire(entity, ev, ...)
end

GlobalExports.entity_prop_reg   = entity_prop_register
GlobalExports.entity_prop_unreg = entity_prop_unregister
GlobalExports.entity_prop_fire  = entity_prop_fire

GlobalExports.entity_event_reg   = entity_event_register
GlobalExports.entity_event_unreg = entity_event_unregister
GlobalExports.entity_event_fire  = entity_event_fire