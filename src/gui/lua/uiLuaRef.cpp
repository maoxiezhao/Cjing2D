#include "uiLuaRef.h"
#include "lua\luaContext.h"

namespace gui {

void UILuaRef::AddCallBack(WIDGET_CALL_BACK_TYPE type, const LuaRef & luaRef)
{
	mLuaRefs[type] = luaRef;
}

/**
*	\brief 执行回调函数
*	\return true 则中断事件传递
*/
bool UILuaRef::DoCallBack(WIDGET_CALL_BACK_TYPE type, LuaObject& object)
{
	auto findIt = mLuaRefs.find(type);
	if (findIt != mLuaRefs.end() && !findIt->second.IsEmpty())
	{
		lua_State* l = findIt->second.GetLuaState();
		findIt->second.Push();
		// func object
		LuaContext::PushUserdata(l, object);
		LuaTools::CallFunction(l, 1, 1, "");

		bool ret = LuaTools::OptBoolean(l, -1, false);
		lua_pop(l, 1);
		return ret;
	}
}

bool UILuaRef::HasCallBack(WIDGET_CALL_BACK_TYPE type)
{
	auto findIt = mLuaRefs.find(type);
	return findIt != mLuaRefs.end();
}

void UILuaRef::Clear()
{
	mLuaRefs.clear();
}

}