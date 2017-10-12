-- this file be load by luaContext

-- class define function --
-- 定义一个类，该类通过new方法创建，并自动调用ctor方法
function class(classname)
	local cls = {}
	cls.__cname = classname
	cls.__index = cls
	if not cls.ctor then 
		cls.ctor = function() 
		end
	end
	
	cls.new = function(...)
		local curClass = cls
		local instance = {}
		setmetatable(instance, curClass)
		instance.class = curClass
		instance:ctor(...)
		return instance
	end
	
	return cls
end