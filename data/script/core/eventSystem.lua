-- 脚本级事件系统
-- 事件系统将每个事件以event和scope为key创建hander
-- hander中保存了callback、custom、scope

-- callback的形式为
-- function callback(event, scope, custom, ...)

local EventSystem = SystemModule("EventSystem")

local _xpcall = xpcall
local _debug_traceback = debug.traceback

------------------------------------------------
-- Utils Funciton
------------------------------------------------

local function traced_pcall(func, ...)
	local function handler(msg)
		return _debug_traceback(msg, 2)
	end
	return _xpcall(func, handler, ...)
end	

local function create_event_set(owner)
	local event_set = {}
	event_set.objects = {}
	return event_set
end

-- 获取object指定scope的handler，返回index和整个hander
local function event_get_handler(object, scope)
	local handlers = object.handlers
	for i,v in ipairs(handlers) do
		if v.scope == scope then 
			return i,v
		end
	end
	return 0, nil
end

-- 注册事件，即添加一个handler到该事件的object中
local function event_set_reg(event_set, event, scope, callback, custom)
	local handler, index

	local objects = event_set.objects
	local object = objects[event]
	if not object then 
		object = {handlers = {}}
		objects[event] = object
	else
		index, handler = event_get_handler(object, scope)
	end

	-- 不存在handler则创建，存在则覆盖
	if not handler then 
		handler = {callback = callback, custom = custom, scope = scope}
		table.insert(object.handlers, handler)
	else
		handler.callback = callback
		handler.custom = custom
		handler.delay_delete = nil
	end
	return true
end

-- 注销事件，即该事件的object中删除指定scope的handler
local function event_set_unreg(event_set, event, scope)
	if not event or not scope then return false end

	local objects = event_set.objects
	local object = objects[event]
	local handlers = object.handlers

	local index, handler = event_get_handler(object, scope)
	if not handler then return false end

	-- 如果是正在执行的事件，则延迟删除
	if object.is_firing then
		handler.delay_delete = true
	else
		table.remove(handlers, index)
	end
	return true
end

-- 触发事件，即该事件的object中所有handler执行
local function event_set_fire(event_set, event, ...)
	local objects = event_set.objects
	local object = objects[event]

	if not event or not object then return false end

	-- 遍历所有handler执行
	object.is_firing = true
	local handlers = object.handlers
	for k, handler in ipairs(handlers) do
		if not handler.delay_delete then 
			local callback, custom = handler.callback, handler.custom
			local scope = handler.scope
			local ok,ret = traced_pcall(callback, event, scope, custom, ...)
			if not ok then 
				util_log_err("Errors on fire event ", event)
			end
		end
	end

	-- 清楚延迟删除的handler
	for i = #handlers, 1, -1 do
		local h = handlers[i]
		if h.delay_delete then table.remove(handlers, i) end
	end
	object.is_firing = false
	return true
end

------------------------------------------------
-- Event Function
------------------------------------------------

local normal_event_set = create_event_set()

function register_event(event, scope, callback, custom)
	return event_set_reg(normal_event_set, event, scope, callback, custom)
end

function unregister_event(event, scope)
	return event_set_unreg(normal_event_set, event, scope)
end

function fire_event(event, ...)
	return event_set_fire(normal_event_set, event, ...)
end

GlobalExports.event_system_register_event = register_event
GlobalExports.event_system_unregister_event = unregister_event
GlobalExports.event_system_fire_event = fire_event