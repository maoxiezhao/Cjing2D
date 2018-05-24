-- 系统级utils库
-- 这里直接将一些Utils函数定义在ENV表上

local env = _ENV

local timer_module_start = Timer.Start
local timer_module_remove = Timer.Remove

local function add_delay_timer(timers, name, delay, repeated, callback, custom )
	if name then 
		local timer = timers[name]
		if timer then 
			timer_module_remove(timer)
			timers[name] = nil
		end
	end

	-- create new timer
	local timer_callback = function()
		return callback(custom)
	end
	local timer = timer_module_start(timers, delay, timer_callback, repeated)
	if timer then 
		timer.name = name
		timers[name] = timer
		return timer
	end
end

local function kill_delay_timer(timers, name)
	local timer = timers[name]
	if timer then 
		timer_module_remove(timer)
		timers[name] = nil
	end
end

local function RegisterTimerFunction(env)
	env._timer = {}
	env.SetTimer = function(name, interval, callback, custom )
		add_delay_timer(env._timer, name, interval, true, callback, custom )
	end

	env.SetDelayTimer = function( name, delay, callback, custom )
		add_delay_timer(env._timer, name, delay, false, callback, custom )
	end

	env.KillTimer = function ( name )
		kill_delay_timer(env._timer, name)
	end
end

local function RegisterStringFunction(env)
	env.StringUTF8Sub = function(str, len)
		if str ==nil then
	        return ""
	    end
	    local lengthUTF8 = #(string.gsub(str, "[\128-\191]", ""))
	    if lengthUTF8 <= len then
	        return str
	    else
	        local matchStr = "^"
	        for var=1, len do
	            matchStr = matchStr..".[\128-\191]*"
	        end
	        local ret = string.match(str, matchStr)
	        return string.format("%s",ret);
	    end
	end
end

local function RegisterMathFunction(env)
	env.IsNan = function(x)
		return x ~= x
	end

end

local function RegisterUtilsFunction(env)
	RegisterTimerFunction(env)
	RegisterStringFunction(env)
	RegisterMathFunction(env)
end

RegisterUtilsFunction(env)