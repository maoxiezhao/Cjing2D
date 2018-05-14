#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"gui\uiStage.h"
#include"gui\widget\frame.h"
#include"gui\lua\uiApi.h"
#include"gui\lua\frameData.h"

namespace gui {

auto GetLuaContext = LuaContext::GetLuaContext;

Frame& GetStageRoot(lua_State* l)
{
	auto& mainStage = UIStage::GetInstance();
	return mainStage.GetRoot();
}

int RegisterFunction(lua_State* l)
{
	/** widget base */
	LuaBindClass<gui::Widget> widgetClass(l, gui::ui_lua_widget_name);
	widgetClass.AddDefaultMetaFunction();

	LuaBindClass<gui::Frame> windowClass(l, gui::ui_lua_frame_name, gui::ui_lua_widget_name);
	windowClass.AddDefaultMetaFunction();
	windowClass.AddFunction("GetRoot", frame_api_get_root);
	windowClass.AddMethod("CreateFrame", frame_api_create_frame);
	windowClass.AddMethod("SetCallBack", frame_api_set_call_back);
	windowClass.AddMethod("ClearCallBacks", &Frame::ClearLuaCallBack);
	windowClass.AddMethod("RemoveChildren", &Frame::RemoveChildren);
	windowClass.AddMethod("RemoveAllChildrens", &Frame::RemoveAllChildrens);
	windowClass.AddMethod("RemoveBySelf", frame_api_remove_by_self);
	windowClass.AddMethod("GetParent", frame_api_get_parent);

	return 0;
}
LUA_FUNCTION_AUTO_BINDER(Frame, RegisterFunction)

/**
*	\brief 获取Root Frame
*/
int frame_api_get_root(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& root = GetStageRoot(l);
		GetLuaContext(l).PushUserdata(l, root);

		return 1;
	});
}

/**
*	\brief 实现Window:CreateFrame("name", frameData )
*/
int frame_api_create_frame(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));

		const std::string& name = LuaTools::CheckString(l, 2);
		FrameData data = FrameData::CheckFrameData(l, 3, WIDGET_TYPE::WIDGET_FRAME);
		auto newFrame = std::make_shared<gui::Frame>(
			data.GetPos().x, data.GetPos().y, 
			data.GetSize().width, data.GetSize().height);
		newFrame->SetWantedPosition(data.GetPos());
		newFrame->SetID(name);
		
		Point2 gridPos = data.GetGridPos();
		frame.SetChildren(newFrame, gridPos.x, gridPos.y, data.GetAlignFlag(), 0);

		GetLuaContext(l).PushUserdata(l, *newFrame);
		return 1;
	});
}

/**
*	\brief 实现 Window:SetCallBack("name", func )
*/
int frame_api_set_call_back(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));

		const std::string& name = LuaTools::CheckString(l, 2);
		LuaRef callback = LuaTools::CheckFunction(l, 3);

		auto type = StringToEnum<WIDGET_CALL_BACK_TYPE>(name);
		frame.SetLuaCallBack(type, callback);
		return 0;
	});
}

/**
*	\brief Window:RemoveBySelf()
*/
int frame_api_remove_by_self(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));
		
		auto parent = frame.GetParentFrame();
		if (parent == nullptr)
		{
			LuaTools::Error(l, "Try to remove root frame.");
			return 0;
		}
		const std::string& id = frame.GetID();
		parent->RemoveChildren(id);

		return 0;
	});
}

/**
*	\brief Window:GetParent()
*/
int frame_api_get_parent(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));

		Frame* parentFrame = frame.GetParentFrame();
		if (parentFrame != nullptr)
			GetLuaContext(l).PushUserdata(l, *parentFrame);
		else
			lua_pushnil(l);

		return 1;
	});
}

}