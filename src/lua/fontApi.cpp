#include"luaContext.h"
#include"game\textDrawable.h"

const string LuaContext::module_font_name = "Text";

const std::map<unsigned int, string> horizontalAlignNames = {
	{ font::TEXT_ALIGN_LEFT,   "left"  },
	{ font::TEXT_ALIGN_CENTER, "center" },
	{ font::TEXT_ALIGN_RIGHT,  "right" }
};

/**
*	\brief 注册video模块
*/
void LuaContext::RegisterFontModule()
{
	static const luaL_Reg functions[] = {
		{ "create",text_api_create },
		{ nullptr,nullptr }
	};

	static const luaL_Reg methods[] = {
		{ "setFont", text_api_set_font },
		{ "getFont", text_api_get_font },
		{ "setText", text_api_set_text },
		{ "getText", text_api_get_text },
		{ "setFontColor", text_api_set_color },
		{ "getFontColor", text_api_get_color },
		{ "setFontSize", text_api_set_size },
		{ "getFontSize", text_api_get_size },
		{ "setLineHeight", text_api_set_line_height },
		{ "setLetterSpacing", text_api_set_letter_spacing },
		{ "draw", text_api_draw },
		{ "getPos", drawable_api_get_pos },		// 下面的方法应该在drawapi实现，派生给sprtie,暂未实现
		{ "setPos", drawable_api_set_pos },		
		{ "runMovement", drawable_api_run_movement },
		{ "getMovment", drawable_api_get_movement },
		{ "stopMovement", drawable_api_stop_movment },
		{nullptr, nullptr}
	};
	
	static const luaL_Reg metamethods[] = {
		{"__gc", userdata_meta_gc},
		{nullptr, nullptr},
	};

	RegisterType(module_font_name, functions, methods, metamethods);
}

/**
*	\brief 将Text压栈
*/
void LuaContext::PushText(lua_State*l, TextDrawable& textDrawable)
{
	PushUserdata(l, textDrawable);
}

/**
*	\brief 检查栈上index索引处是否为textDrawable userdata
*	\return the sprite
*/
std::shared_ptr<TextDrawable> LuaContext::CheckTextDrawable(lua_State*l, int index)
{
	return std::static_pointer_cast<TextDrawable>(CheckUserdata(l, index, module_font_name));
}

/**
*	\brief 返回栈上index索引处是否textDrawable
*	\return true 如果是textDrawable
*/
bool LuaContext::IsTextDrawable(lua_State*l, int index)
{
	return IsUserdata(l, index, module_font_name);
}

/**
*	\brief 实现cjing.Text.create()
*
*	对于传入的第一个参数应为一个表，表中如果存在下列属性则赋值
*	font font的在fonAltas中载入的名字
*	horizontalAlign 水平对齐模式
*	fontSize 绘制的字体大小
*	fontColor 绘制的字体颜色
*	lineHeight 行高
*	letterSpacing 水平字间距
*/
int LuaContext::text_api_create(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawablePtr textDrawable = std::make_shared<TextDrawable>();

		// property table
		const string& fontID = LuaTools::CheckFieldStringByDefault(l, 1, "fontName", font::DEFAULT_FONT_NAME);
		int fontSize = LuaTools::CheckFieldIntByDefault(l, 1, "fontSize", 0);
		int lineHeight = LuaTools::CheckFieldIntByDefault(l, 1, "lineHeight", 0);
		int letterSpacing = LuaTools::CheckFieldIntByDefault(l, 1, "letterSpacing", 0);
		const Color4B fontColor = LuaTools::CheckFieldColorByDefault(l, 1, "fontColor", Color4B::WHITE);
		// ....
		auto fontPtr = font::FontAtlas::GetInstance().GetFontByName(fontID);
		textDrawable->SetFont(fontPtr);
		textDrawable->SetTextColor(fontColor);

		if (lineHeight > 0)
			textDrawable->SetLineHeight(lineHeight);
		if (fontSize > 0)
			textDrawable->SetFontSize(fontSize);
		if (letterSpacing > 0)
			textDrawable->SetLetterSpacing(letterSpacing);

		GetLuaContext(l).AddDrawable(textDrawable);
		PushText(l, *textDrawable);

		return 1;
	});
}