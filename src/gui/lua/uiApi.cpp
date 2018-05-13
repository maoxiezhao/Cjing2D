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
	windowClass.AddMethod("CreateFrame", frame_api_create);

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
*	\brief 实现cjing.Window.create("name", frameData )
*/
int frame_api_create(lua_State* l)
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

}