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
-- 插件全局接口表
local addon_exports = {}

-- 插件特殊支持的env
local addon_env_list = {
	-- uitls log
	["util_log_info"] = "util_log_info",
	["util_log_warn"] = "util_log_warn",
	["util_log_err"]  = "util_log_err",

	["GetGame"]       = "game_manager_get_game",
	["GetPlayer"]     = "game_manager_get_player",
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
	local addon_env = {AddonExports = {}}
	setmetatable(addon_env, {__index = env})

	local addon_path = "script/addons/" .. name .. ".lua"
	local ret, err = traced_pcall(SystemDoFile, addon_path, addon_env)
	if not ret then 
		util_log_warn("Load addon " .. name .. " failed " .. err)
		return 
	end

	local addon_export = addon_env.AddonExports
	-- 创建addon实例
	local addon = {
		name = name, 
		env = addon_env,
		export = addon_export,
	}
	setmetatable(addon, {__index = function(t, k)
		local env = rawget(t, "env");
		local v = rawget(env, k);
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
	return addon
end

local function adddon_manager_load_addons()
	local addons = game_content_get_templ("addonLists")
	if addons then 
		local addons_list = addons.cfgs
		if not addons_list then 
			util_log_err("Failed to load addon list.")
			return
		end

		util_log_info("************** Start loading addons **************")
		for _,name in pairs(addons_list) do
			addon_manager_load_addon(name)
		end
		util_log_info("************** Finish loading addons **************")
	end
end

local function adddon_manager_unload_addons()

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

end

function AddonManager.LoadAddons(addons)

end

GlobalExports.addon_manager_init  = AddonManager.OnInitialize
GlobalExports.addon_manager_update = AddonManager.OnUpdate
GlobalExports.addon_manager_uninit = AddonManager.OnUnitialize