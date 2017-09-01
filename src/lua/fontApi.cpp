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
		{ "load",text_api_load_font},
		{ "loadDefault",text_api_load_font_default},
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
		{ "setHorizontalAlign",text_api_set_horizontal_align },
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

/**
*	\brief 实现cjing.Text.create()
*
*	对于传入的第一个参数应为一个表，该表用于配置fontConfig
*	{name, fontSize, letterSpacing, lineHeight, codeRanges}
*    name 加载的字体名字
*	 fontSize 加载的字体字号
*	 letterSpacing 字间距
*	 lineHeight 行高
*	 codeRanges 加载utf8范围，该值为一个表
*	   {{codeBegin, codeEnd},{codeBegin, codeEnd} ...}
*/
int LuaContext::text_api_load_font(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {

		return 0;
	});
}

/**
*	\brief 实现cjing.Text.loadDefault()
*
*	加载默认字体，调用fontAltas::LoadDefaultFont
*/
int LuaContext::text_api_load_font_default(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		font::FontAtlas::GetInstance().LoadDefaultFont();

		return 0;
	});
}

/**
*	\brief 实现Text:setFont(fontName)
*
*	fontName是font的ID,该font务必已经在fontAltas中加载
*/
int LuaContext::text_api_set_font(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& text = *CheckTextDrawable(l, 1);
		const string& fontName = LuaTools::CheckString(l, 2);
		auto fontPtr = font::FontAtlas::GetInstance().GetFontByName(fontName);
		if (fontPtr != nullptr)
		{
			text.SetFont(fontPtr);
		}

		return 0;
	});
}

/**
*	\brief 实现Text:getFont()
*	\return 将font的fontName压栈
*/
int LuaContext::text_api_get_font(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& text = *CheckTextDrawable(l, 1);
		auto fontPtr = text.getFont();
		
		string fontName;
		if (fontPtr != nullptr)
		{
			fontName = font::FontAtlas::GetInstance().GetNameByFont(fontPtr);
		}
		lua_pushstring(l, fontName.c_str());
		return 1;
	});
}

/**
*	\brief 实现Text:setText(text)
*
*	设置需要绘制的文本,必须传入的字符串是utf8格式
*/
int LuaContext::text_api_set_text(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		const string& text = LuaTools::CheckString(l, 2);
		textDrawable.SetText(text);

		return 0;
	});
}

int LuaContext::text_api_get_text(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		const string& text = textDrawable.GetText();
		lua_pushstring(l, text.c_str());

		return 1;
	});
}

/**
*	\brief 实现Text:setFontColor({r,g,b,a})
*/
int LuaContext::text_api_set_color(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		const Color4B& color = LuaTools::CheckColor(l, 2);
		textDrawable.SetTextColor(color);

		return 0;
	});
}

/**
*	\brief 实现Text:getFontColor()
*	\return 返回r,g,b,a
*/
int LuaContext::text_api_get_color(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		const Color4B& color = textDrawable.GetTextColor();
		lua_pushinteger(l, (int)color.r);
		lua_pushinteger(l, (int)color.g);
		lua_pushinteger(l, (int)color.r);
		lua_pushinteger(l, (int)color.g);
		
		return 4;
	});
}

int LuaContext::text_api_set_size(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		int fontSize = LuaTools::CheckInt(l, 2);
		textDrawable.SetFontSize(fontSize);

		return 0;
	});
}

int LuaContext::text_api_get_size(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		int fontSize = textDrawable.GetFontSize();
		lua_pushinteger(l, fontSize);

		return 1;
	});
}

int LuaContext::text_api_set_line_height(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		int lineHeight = LuaTools::CheckInt(l, 2);
		textDrawable.SetLineHeight(lineHeight);

		return 0;
	});
}

int LuaContext::text_api_set_letter_spacing(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		int letterSpacing = LuaTools::CheckInt(l, 2);
		textDrawable.SetLetterSpacing(letterSpacing);

		return 0;
	});
}

int LuaContext::text_api_set_horizontal_align(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		const string& align = LuaTools::CheckString(l, 2);

		unsigned int alignCode = font::TEXT_ALIGN_LEFT;
		for (auto& kvp : horizontalAlignNames)
		{
			if (kvp.second == align)
			{
				alignCode = kvp.first;
			}
		}
		textDrawable.SetTextHorizontalAlign(alignCode);

		return 0;
	});
}

int LuaContext::text_api_draw(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		TextDrawable& textDrawable = *CheckTextDrawable(l, 1);
		textDrawable.Draw();

		return 0;
	});
}

