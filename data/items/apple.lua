local item = ...

-- item 脚本 
-- item脚本解释了item创建时的一些属性以及获取道具时的操作
--
-- item本身在创建时会根据是否HasLua决定是否载入脚本,对于无特殊
-- 操作，而仅仅是保存道具数量的item应不载入lua脚本

-- 创建时动作
function item:onCreated()
	self:SetAutoPicked(false)
	self:SetShadow(false)
	--self:setFlow(false)
end

-- 获取物品时调用
function item:onObtained(itemName, itemCount)
	print("Item Test.")
end
