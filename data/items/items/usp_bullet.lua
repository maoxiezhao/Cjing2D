local item = ...
local game = item:GetGame()

-- item 脚本 
-- item脚本解释了item创建时的一些属性以及获取道具时的操作
--
-- item本身在创建时会根据是否HasLua决定是否载入脚本,对于无特殊
-- 操作，而仅仅是保存道具数量的item应不载入lua脚本

-- 注意区分item下的bullet和entities下的bullet，这里是子弹道具

-- 创建时动作
function item:onCreated()
	self:SetAutoPicked(false)
	self:SetShadow(false)
	--self:setFlow(false)

end

-- 获取物品时调用
function item:onObtained(itemName, itemCount)
	-- 如果当前武器没有子弹，则默认执行换弹操作
	
end

