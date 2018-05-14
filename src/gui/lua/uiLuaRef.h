#pragma once

#include"lua\luaRef.h"
#include"gui\widget\widgetInfo.h"

#include<map>

namespace gui
{
/**
*	\brief UI Lua�ص�������
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
	*	\brief ִ�лص�����
	*	\return true ���ж��¼�����
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