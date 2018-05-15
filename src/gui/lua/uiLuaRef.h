#pragma once

#include"lua\luaObject.h"
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

	void AddCallBack(WIDGET_CALL_BACK_TYPE type, const LuaRef& luaRef);
	bool DoCallBack(WIDGET_CALL_BACK_TYPE type, LuaObject& object);
	bool HasCallBack(WIDGET_CALL_BACK_TYPE type);
	void Clear();

private:
	std::map<WIDGET_CALL_BACK_TYPE, LuaRef> mLuaRefs;
};
}