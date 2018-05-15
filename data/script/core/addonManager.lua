-- 界面插件管理器
local AddonManager = SystemModule("AddonManager")

local _xpcall = xpcall
local _debug_traceback = debug.traceback

local function traced_pcall(func, ...)
	local function msgHander(msg)
		return _debug_traceback(msg, 2)
	end
	return _xpcall(func, msgHander, ...)
end

if not CLIENT_LUA_DEBUG then 
	traced_pcall = pcall
end

------------------------------------------------------
-- 插件列表
local cur_addon_lists = nil

-- 插件全局接口表
local addon_exports = {}

-- 插件特殊支持的env
local addon_env_list = {
	-- uitls log
	["util_log_info"] = "util_log_info",
	["util_log_warn"] = "util_log_warn",
	["util_log_err"]  = "util_log_err",

	-- timer func
	["SetTimer"] 	  = "SetTimer",
	["SetDelayTimer"] = "SetDelayTimer",
	["KillTimer"] 	  = "KillTimer",

	-- some game func
	["StartGame"]	  = "game_manager_start_debug",
	["GetGame"]       = "game_manager_get_game",
	["GetPlayer"]     = "game_manager_get_player",

	-- ui modules
	["PathMovement"]  = "PathMovement",
	["Frame"]	      = "Frame",
}

-- 界面的addon的env需要做一个限制
local function addon_create_env(genv)
	local env = {}
	for _,k in pairs(ORGIN_ENV_LIST) do
		env[k] = genv[k]
	end

	for name,k in pairs(addon_env_list) do
		env[name] = genv[k]
	end

	setmetatable(env, {__index = addon_exports})
	return env
end

local function addon_load_addon(name, env)
	-- 创建addon环境
	local addon_env = {AddonExports = {}, [name] = {}}
	setmetatable(addon_env, {__index = env})

	local addon_path = "script/addons/" .. name .. ".lua"
	local ret, err = traced_pcall(SystemDoFile, addon_path, addon_env)
	if not ret then 
		util_log_warn("Load addon " .. name .. " failed " .. err)
		return 
	end

	local addon_inst = addon_env[name] or {}
	local addon_export = addon_env.AddonExports

	-- 创建addon实例
	local addon = {
		name = name, 
		env = addon_env,
		inst = addon_inst,
		export = addon_export,
	}
	-- 可以通过addon实例访问到addon内部的env、inst、export接口
	setmetatable(addon, {__index = function(t, k)
		local env = rawget(t, "env");
		local v = rawget(env, k);
		if v then return v end

		local inst = rawget(t, "inst");
		local v = rawget(inst, k);
		if v then return v end

		local exports = rawget(t, "export");
		local v = rawget(exports, k);
		if v then return v end
	end})

	addon_exports[name] = addon_export

	return addon
end

-------------------------------------------------------
-- Addon Instacnce
-------------------------------------------------------

local addon_manager = nil
local function create_addon_manager()
	local addon_mgr = {
		env = nil,
		addons = {},
	}
	return addon_mgr
end

local function addon_manager_load_addon(name)
	util_log_info("Load addon "..  name)
	local cur_addons = addon_manager.addons
	if cur_addons[name] then
		util_log_warn("Addon "..  name .. " has already loaded.")
		return 
	end
	-- 额外的依赖关系处理

	-- 加载adddon
	local env = addon_manager.env
	local addon = addon_load_addon(name, env)
	if not addon then 
		util_log_warn("Addon "..  name .. " load failed.")
		return 
	end

	if addon.OnLoad then 
		traced_pcall(addon.OnLoad)
	end

	cur_addons[name] = addon
	table.insert(cur_addons, addon) -- 插入表中是为了保证先后顺序

	return addon
end

-- 获取addon列表
local function addon_manager_load_addon_list()
	local addons = game_content_get_templ("addonLists")
	if addons then 
		local addons_list = addons.cfgs
		if not addons_list then 
			return
		end
		return addons_list
	end
end

-- 加载全部插件
local function adddon_manager_load_addons()
	if not cur_addon_lists then 
		local addon_list = addon_manager_load_addon_list()
		if not addon_list then 
			util_log_err("Failed to load addon list.")
			return 
		end
		cur_addon_lists = addon_list
	end

	util_log_info("************** Start loading addons **************")
	for _,name in pairs(cur_addon_lists) do
		addon_manager_load_addon(name)
	end
	util_log_info("************** Finish loading addons **************")
end

-- 卸载指定插件
local function addon_manager_unload_addon(name)
	local addon = addon_manager.addons[name]
	if not addon then return end

	util_log_info("Unload addon "..  name)

	if addon.OnUnLoad then 
		traced_pcall(addon.OnUnLoad)
	end

	-- clear
	addon_exports[name] = nil
	addon_manager.addons[name] = nil

	for i, v in ipairs(addon_manager.addons) do
		if addon == v then 
			table.remove(addon_manager.addons, i)
			break
		end
	end
end

-- 卸载全部插件
local function adddon_manager_unload_addons()
	if cur_addon_lists then 
		util_log_info("************** Start unloading addons **************")
		for _,name in pairs(cur_addon_lists) do
			addon_manager_unload_addon(name)
		end
		util_log_info("************** Finish unloading addons **************")
	end
end

-- 更新所有的插件
local function adddon_manager_update_addons()
	local cur_addons = addon_manager.addons
	for _, addon in ipairs(cur_addons) do
		if addon.OnUpdate then 
			traced_pcall(addon.OnUpdate)
		end
	end
end

--------------------------------------------------------
-- Addon Interface
--------------------------------------------------------

function AddonManager.OnInitialize()
	util_log_info("[lua] AddonManager:Init.")

	addon_manager = create_addon_manager()
	addon_manager.env = addon_create_env(_ENV)

	adddon_manager_load_addons()
end

function AddonManager.OnUnitialize()
	util_log_info("[lua] AddonManager:UnInit.")

	adddon_manager_unload_addons()
end

function AddonManager.OnUpdate()
	adddon_manager_update_addons()
end

function AddonManager.OnRootStart()
	local root = addon_manager.addons["Root"]
	if not root or not root.OnRootStart then 
		util_log_err("The addon root or root.OnRootStart is nil.")
		return 
	end
	traced_pcall(root.OnRootStart)
end

GlobalExports.addon_manager_init  = AddonManager.OnInitialize
GlobalExports.addon_manager_update = AddonManager.OnUpdate
GlobalExports.addon_manager_uninit = AddonManager.OnUnitialize
GlobalExports.addon_manager_root_start = AddonManager.OnRootStart