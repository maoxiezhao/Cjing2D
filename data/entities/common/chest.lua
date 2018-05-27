local Chest = ...
local Game = game_manager_get_game()
local EntitySystem = SystemImport("EntitySystem")

-- 普通的宝箱
-- 可以直接打开
-- 打开后创建pickable

-- 判断是否能打开
function Chest:OnCanOpen()
	return true	-- 普通宝箱默认全部可以打开
end

-- 成功打开宝箱
function Chest:OnOpenSucceed(entity)
	local pos = self:GetPos()
	local items = StringSplit(self:GetItems())
	if #items <= 0 then 
		return 
	end

	for _, item in ipairs(items) do 
		local data  = {
		  x = pos[1],
		  y = pos[2],
		  layer = self:GetLayer(),
		  itemName = item,
		  itemCount = 1,
		}
	
	 	EntitySystem.CreatePickable(data)
	end

 	entity:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "^_^")
end


-- 打开宝箱失败
function Chest:OnOpenFailed(entity)
	entity:SetProperty(ENTITY_PROPERTY_HEAD_LABEL, "T_T")
end

function Chest:OnCreated()
	
end
