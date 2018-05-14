#pragma once

#include"lua\luaRef.h"
#include"gui\widget\widgetInfo.h"

#include<map>

namespace gui
{
/**
*	\brief UI Lua回调函数集
*/
class UILuaRef
{
public:
	UILuaRef() = default;

	void AddCallBack(WIDGET_CALL_BACK_TYPE type, const LuaRef& luaRef)
	{
		mLuaRefs[type] = luaRef;
	}

	/**
	*	\brief 执行回调函数
	*	\return true 则中断事件传递
	*/
	bool DoCallBack(WIDGET_CALL_BACK_TYPE type)
	{
		auto findIt = mLuaRefs.find(type);
		if (findIt != mLuaRefs.end() && !findIt->second.IsEmpty())
		{
			lua_State* l = findIt->second.GetLuaState();
			findIt->second.Push();
			LuaTools::CallFunction(l, 0, 1, "");

			bool ret = LuaTools::OptBoolean(l, -1, false);
			lua_pop(l, 1);
			return ret;
		}
	}

	bool HasCallBack(WIDGET_CALL_BACK_TYPE type)
	{
		auto findIt = mLuaRefs.find(type);
		return findIt != mLuaRefs.end();
	}

	void Clear()
	{
		mLuaRefs.clear();
	}
private:
	std::map<WIDGET_CALL_BACK_TYPE, LuaRef> mLuaRefs;
};
}