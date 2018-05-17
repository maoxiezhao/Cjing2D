-- 对话模块
-- 提供调用对话的全局接口，同时会响应Addon的界面事件

-- TODO 对话系统作为Core系统本身是不合适的，考虑实现未来实现Module
-- 管理，将所有的Module类似Addon方式管理，游戏具体功能则实现为Module

local DialogSystem = SystemModule("DialogSystem")

local _xpcall = xpcall
local _debug_traceback = debug.traceback

local dialogDatas = nil
------------------------------------------------------

function DialogSystem.OpenDialogByID(id)
	if dialogDatas then 
		return dialogDatas[id]
	end
end

function DialogSystem.Initialize()
	util_log_info("[lua] DialogSystem:Init.")
	local dialogInfo = game_content_get_templ("DialogsList")
	if dialogInfo then 
		dialogDatas = dialogInfo.dialogs
	end
end

function DialogSystem.Uninitialize()
	util_log_info("[lua] DialogSystem:UnInit.")
end

function DialogSystem.StartDialog(ids, callback)
	event_system_fire_event(EVENT_GAME_START_DIALOG, ids, callback)
end

function DialogSystem.StartQuest(ids, callbacks)
	event_system_fire_event(EVENT_GAME_START_QUESTIONS, ids, callbacks)
end

GlobalExports.dialog_system_init = DialogSystem.Initialize
GlobalExports.dialog_system_uninit = DialogSystem.Uninitialize
GlobalExports.dialog_system_start_dialog = DialogSystem.StartDialog