local item = ...
local game = item:getGame()

-- item 脚本 
-- item脚本解释了item创建时的一些属性以及获取道具时的操作
--
-- item本身在创建时会根据是否HasLua决定是否载入脚本,对于无特殊
-- 操作，而仅仅是保存道具数量的item应不载入lua脚本

-- 创建时动作
function item:onCreated()
	self:setAutoPicked(false)
	self:setShadow(false)
	--self:setFlow(false)
	
	-- item define
	self._heartLife = 3
end

-- 获取物品时调用
function item:onObtained(itemName, itemCount)
	local curLife = game:getLife()
	local maxLife = game:getMaxLife()

	if curLife == maxLife then 
		return false
	end

	curLife = curLife + self._heartLife
	if curLife >= maxLife then 
		curLife = maxLife
	end

	game:setLife(curLife)
end
