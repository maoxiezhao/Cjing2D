-- 输入模块
-- 目前仅作为输入、输出时响应Event函数

-- TODO 将会创建各个Input Mapping表，根据当前模式
-- 选择Input Mapping， 在InputMapping表中，会创建
-- {key, callback}的handle，在update中检查者mapping中
-- key是否按下，如果按下则响应回调。与InputEvent兼容

local InputSystem = SystemModule("InputSystem")

local _xpcall = xpcall
local _debug_traceback = debug.traceback

------------------------------------------------------

local key_mapping = {}

local function create_key_handle(key, callback, set)
	local key_set = key_mapping[set]
	if not key_set then 
		return false
	end

	local handle = {}
	handle.key = key
	handle.callback = callback
	table.insert(key_set, handle)

	return true
end


function InputSystem.OnInputKeyDown( key, modify )
	event_system_fire_event(EVENT_INPUT_KEY_DOWN, key, modify)
end

function InputSystem.OnInputKeyUp( key, modify )
	event_system_fire_event(EVENT_INPUT_KEY_UP, key, modify)
end

SystemExports[CLIENT_LUA_INPUT_KEY_DOWN] = InputSystem.OnInputKeyDown
SystemExports[CLIENT_LUA_INPUT_KEY_UP] = InputSystem.OnInputKeyUp