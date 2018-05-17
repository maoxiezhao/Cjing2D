#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"gui\lua\uiApi.h"
#include"gui\lua\frameData.h"
#include"gui\widget\frame.h"
#include"gui\widget\button.h"
#include"gui\widget\image.h"
#include"gui\widget\label.h"
#include"gui\uiStage.h"
#include"gui\core\uiRender.h"

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
	widgetClass.AddMethod("SetVisible", &Widget::SetVisibility);
	widgetClass.AddMethod("GetVisible", &Widget::GetVisibility);
	widgetClass.AddMethod("SetSize", &Widget::SetSize);
	widgetClass.AddMethod("GetSize", &Widget::GetSize);
	widgetClass.AddMethod("SetPos", &Widget::SetWantedPosition);
	widgetClass.AddMethod("GetPos", &Widget::GetPositon);
	widgetClass.AddMethod("SetCallBack", frame_api_set_call_back);
	widgetClass.AddMethod("CaptureMouse", frame_api_capture_mouse);
	widgetClass.AddMethod("ClearCallBacks", &Widget::ClearLuaCallBack);
	widgetClass.AddMethod("StopMovement", &Widget::StopMovement);
	widgetClass.AddMethod("ToTop", &Widget::ToTopByParent);

	/** frame base */
	LuaBindClass<gui::Frame> windowClass(l, gui::ui_lua_frame_name, gui::ui_lua_widget_name);
	windowClass.AddDefaultMetaFunction();
	windowClass.AddFunction("GetRoot", frame_api_get_root);
	windowClass.AddFunction("SetDebug", frame_api_set_debug);
	windowClass.AddFunction("LoadFont", &UIRender::LoadFontTTF);
	windowClass.AddFunction("AddCustom", frame_api_add_custom_method);
	windowClass.AddFunction("DrawDebugBoard", &UIRender::SetDebugDraw);

	windowClass.AddMethod("CreateFrame", frame_api_create_frame);
	windowClass.AddMethod("CreateButton", frame_api_create_button);
	windowClass.AddMethod("CreateImage", frame_api_create_image);
	windowClass.AddMethod("CreateLabel", frame_api_create_label);

	windowClass.AddMethod("RemoveChildren", &Frame::RemoveChildren);
	windowClass.AddMethod("RemoveAllChildrens", &Frame::RemoveAllChildrens);
	windowClass.AddMethod("RemoveBySelf", frame_api_remove_by_self);
	windowClass.AddMethod("GetParent", frame_api_get_parent);

	/** image */
	LuaBindClass<gui::Image> imageClass(l, gui::ui_lua_image_name, gui::ui_lua_widget_name);
	imageClass.AddDefaultMetaFunction();
	imageClass.AddMethod("SetImage", &Image::SetImagePath);
	imageClass.AddMethod("GetImage", &Image::GetImagePath);

	/** button */
	LuaBindClass<gui::Image> buttonClass(l, gui::ui_Lua_button_name, gui::ui_lua_widget_name);
	buttonClass.AddDefaultMetaFunction();

	/** label */
	LuaBindClass<gui::Label> labelClass(l, gui::ui_lua_lable_name, gui::ui_lua_widget_name);
	labelClass.AddDefaultMetaFunction();
	labelClass.AddMethod("SetMultline",   &Label::SetMultline);
	labelClass.AddMethod("GetMultline",   &Label::GetMultline);
	labelClass.AddMethod("SetFontSize",   &Label::SetFontSize);
	labelClass.AddMethod("GetFontSize",   &Label::GetFontSize);
	labelClass.AddMethod("SetLineHeight", &Label::SetLineHeight);
	labelClass.AddMethod("GetLineHeight", &Label::GetLineHeight);
	labelClass.AddMethod("SetFontColor",  &Label::SetFontColor);
	labelClass.AddMethod("GetFontColor",  &Label::GetFontColor);
	labelClass.AddMethod("SetFontFace",   &Label::SetFontFace);
	labelClass.AddMethod("GetFontFace",   &Label::GetFontFace);
	labelClass.AddMethod("SetText",       &Label::SetText);
	labelClass.AddMethod("GetText",       &Label::GetText);
	labelClass.AddMethod("SetTextAlign",  &Label::SetAlign);
	labelClass.AddMethod("GetTextAlign",  &Label::GetAlign);

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
*	\brief frame.AddCustom(name, func)
*/
int frame_api_add_custom_method(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaTools::CheckString(l, 1);
		LuaTools::CheckFunction(l, 2);
						// name func 
		luaL_getmetatable(l, ui_lua_frame_name.c_str());
		if (lua_isnil(l, -1))
		{
			LuaTools::Error(l, "The module of frame is not created." );
			return 0;
		}
						// name func meta
		lua_pushvalue(l, 1);
						// name func meta name
		lua_pushvalue(l, 2);
						// name func meta name func						
		lua_rawset(l, -3);
						// name func meta
		lua_pop(l, 1);

		return 0;
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

		if (newFrame == nullptr)
			Debug::Error("Failed to create frame " + name);

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
		Widget& widget = *std::static_pointer_cast<Widget>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_widget_name));

		const std::string& name = LuaTools::CheckString(l, 2);
		LuaRef callback = LuaTools::CheckFunction(l, 3);

		auto type = StringToEnum<WIDGET_CALL_BACK_TYPE>(name);
		widget.SetLuaCallBack(type, callback);
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

int frame_api_set_debug(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		bool debuged = LuaTools::CheckBoolean(l, 1);
		Widget::SetDebugDraw(debuged);

		return 0;
	});
}

int frame_api_capture_mouse(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Widget& widget = *std::static_pointer_cast<Widget>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_widget_name));

		bool captured = LuaTools::CheckBoolean(l, 2);
		Frame* root = widget.GetRoot();
		if (root)
			root->MouseCaptrue(captured);

		return 0;
	});
}

/**
*	\brief Window:CreateButton(name, data)
*/
int frame_api_create_button(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));

		const std::string& name = LuaTools::CheckString(l, 2);
		FrameData data = FrameData::CheckFrameData(l, 3, WIDGET_TYPE::WIDGET_BUTTON);

		auto button = std::make_shared<gui::Button>();
		button->Connect();
		button->SetWantedPosition(data.GetPos());
		button->SetSize(data.GetSize());
		button->SetID(name);
		button->SetLabel(data.GetValueString("label"));

		Point2 gridPos = data.GetGridPos();
		frame.SetChildren(button, gridPos.x, gridPos.y, data.GetAlignFlag(), 0);

		GetLuaContext(l).PushUserdata(l, *button);
		return 1;
	});
}

/**
*	\brief Window:CreateImage(name, data)
*/
int frame_api_create_image(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));

		const std::string& name = LuaTools::CheckString(l, 2);
		FrameData data = FrameData::CheckFrameData(l, 3, WIDGET_TYPE::WIDGET_IMAGE);
		const std::string path = data.GetValueString("path");

		auto image = std::make_shared<gui::Image>();
		image->SetWantedPosition(data.GetPos());
		image->SetSize(data.GetSize());
		image->SetID(name);
		image->SetImagePath(path);

		Point2 gridPos = data.GetGridPos();
		frame.SetChildren(image, gridPos.x, gridPos.y, data.GetAlignFlag(), 0);

		GetLuaContext(l).PushUserdata(l, *image);
		return 1;
	});

}
int frame_api_create_label(lua_State * l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Frame& frame = *std::static_pointer_cast<Frame>(
			GetLuaContext(l).CheckUserdata(l, 1, ui_lua_frame_name));

		const std::string& name = LuaTools::CheckString(l, 2);
		FrameData data = FrameData::CheckFrameData(l, 3, WIDGET_TYPE::WIDGET_LABEL);
		const std::string text = data.GetValueString("text");

		auto label = std::make_shared<gui::Label>();
		label->SetWantedPosition(data.GetPos());
		label->SetSize(data.GetSize());
		label->SetID(name);
		label->SetText(text);

		Point2 gridPos = data.GetGridPos();
		frame.SetChildren(label, gridPos.x, gridPos.y, data.GetAlignFlag(), 0);

		GetLuaContext(l).PushUserdata(l, *label);
		return 1;
	});
}
}