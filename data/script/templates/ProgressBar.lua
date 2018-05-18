-- 进度条模板
-- 进度条模板应该分为底图，进度图和Mask遮罩图，通过改变遮罩图大小来
-- 实现进度改变

-- 目前未支持Mask，仅仅只是改变进度图大小

--------------------------------------------
-- Progress image function register
--------------------------------------------
local imageTyps = {"Border", "Progress", "Mask"}

-- 创建了3个SetImage函数
-- SetBorderImage(path, pos, size)
-- SetProgressImage(path, pos, size)
-- SetMaskImage(path, pos, size)
local function register_image_set_image(root, name, img)
	local func_name = "Set" .. name .. "Image"
	root[func_name] = function(root, path, pos, size)
		if not root or not img then return end 

		if path then img:SetImage(path) end
		if pos then img:SetPos(pos) end
		if size then img:SetSize(size) end
	end
end

local function register_image_func(root)
	local images = root._images
	for k, name in pairs(imageTyps) do
		local img = images[name]	-- img must exists

		register_image_set_image(root, name, img)
	end
end

--------------------------------------------
-- Progress utils
--------------------------------------------
local function clamp_progress_value(v, min, max)
	if min > max then return end
	v = math.max(v, min)
	v = math.min(v, max)
	return v
end

local function set_image_width_by_value(root, cur, min, max)
	local image = root._images["Progress"]
	if image then 
		image:GetSize()
		--local size = image:GetSize()
		--print(size, size[1], size[2])
	end
end

--------------------------------------------
-- Progress metatable
--------------------------------------------
local ProgressMT = {
	-- 创建按钮的各个控件
	Initialize = function(root, data)
		-- clear data pos
		data.pos = nil

		local images = {}
		local border_img    = root:CreateImage("border_img",    data)
		local progress_img  = root:CreateImage("progress_img",  data)
		local mask_img      = root:CreateImage("mask_img",      data)

		images["Border"]   = border_img
		images["Progress"] = progress_img
		images["Mask"] 	   = mask_img
		root._images = images

		register_image_func(root)

		-- data define
		root:ResetValue()

		return true
	end,	

	ResetValue = function(root, min, max, cur)
		root._min = min or 0
		root._max = max or 0	
		cur = cur or 0
		root:SetValue(cur)
	end,

	SetValue = function(root, value, is_anim)
		local new_value = clamp_progress_value(value, root._min, root._max)
		local old_value = root._cur 

		-- 先做测试避免设置重复值
		if new_value == old_value then
			return 
		end
		old_value = old_value or 0
		local is_reduce = new_value < old_value

		set_image_width_by_value(root, new_value, root._min, root._max)

		root._cur = new_value
	end,
}

local function create_progress_bar(parent, name, barData)
	local frame = parent:CreateFrame(name, barData)
	if not frame then 
		goto ERROR
	end

	-- set button metatable
	for k,v in pairs(ProgressMT) do 
		frame[k] = v
	end

	-- initialze
	if not frame:Initialize(barData) then
		goto ERROR
	end

	if frame then
		return frame
	end
::ERROR::
	util_log_err("Failed to create progress bar.")
	return nil
end

----------------------------------------------

function OnCreate(parent, name, barData)
	return create_progress_bar(parent, name, barData)
end
