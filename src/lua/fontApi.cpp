#include"luaContext.h"
#include"game\textDrawable.h"

const string LuaContext::module_font_name = "Text";

const std::map<unsigned int, string> horizontalAlignNames = {
	{ font::TEXT_ALIGN_LEFT,   "left"  },
	{ font::TEXT_ALIGN_CENTER, "center" },
	{ font::TEXT_ALIGN_RIGHT,  "right" }
};

/**
*	\brief ע��videoģ��
*/
void LuaContext::RegisterFontModule()
{
	static const luaL_Reg functions[] = {
		{ "create",text_api_create },
		{ "load",text_api_load_font},
		{ "loadDefault",text_api_load_font_default},
		{ "loadDefaultEn", text_api_load_font_default_en },
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
		{ "getPos", drawable_api_get_pos },		// ����ķ���Ӧ����drawapiʵ�֣�������sprtie,��δʵ��
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
*	\brief ��Textѹջ
*/
void LuaContext::PushText(lua_State*l, TextDrawable& textDrawable)
{
	PushUserdata(l, textDrawable);
}

/**
*	\brief ���ջ��index�������Ƿ�ΪtextDrawable userdata
*	\return the sprite
*/
std::shared_ptr<TextDrawable> LuaContext::CheckTextDrawable(lua_State*l, int index)
{
	return std::static_pointer_cast<TextDrawable>(CheckUserdata(l, index, module_font_name));
}

/**
*	\brief ����ջ��index�������Ƿ�textDrawable
*	\return true �����textDrawable
*/
bool LuaContext::IsTextDrawable(lua_State*l, int index)
{
	return IsUserdata(l, index, module_font_name);
}

/**
*	\brief ʵ��cjing.Text.create()
*
*	���ڴ���ĵ�һ������ӦΪһ�������������������������ֵ
*	font font����fonAltas�����������
*	horizontalAlign ˮƽ����ģʽ
*	fontSize ���Ƶ������С
*	fontColor ���Ƶ�������ɫ
*	lineHeight �и�
*	letterSpacing ˮƽ�ּ��
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
*	\brief ʵ��cjing.Text.create()
*
*	���ڴ���ĵ�һ������ӦΪһ�����ñ���������fontConfig
*	{name, fontSize, letterSpacing, lineHeight, codeRanges}
*    name ���ص���������
*	 fontSize ���ص������ֺ�
*	 letterSpacing �ּ��
*	 lineHeight �и�
*	 codeRanges ����utf8��Χ����ֵΪһ����
*	   {{codeBegin, codeEnd},{codeBegin, codeEnd} ...}
*/
int LuaContext::text_api_load_font(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Text.loadDefault()
*
*	����Ĭ�����壬����fontAltas::LoadDefaultFont
*/
int LuaContext::text_api_load_font_default(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		font::FontAtlas::GetInstance().LoadDefaultFont();

		return 0;
	});
}


/**
*	\brief ʵ��cjing.Text.loadDefaultEn()
*
*	����Ĭ��Ӣ�����壬����fontAltas::LoadDefaultFontEn
*/
int LuaContext::text_api_load_font_default_en(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		font::FontAtlas::GetInstance().LoadDefaultFontEn();

		return 0;
	});
}

/**
*	\brief ʵ��Text:setFont(fontName)
*
*	fontName��font��ID,��font����Ѿ���fontAltas�м���
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
*	\brief ʵ��Text:getFont()
*	\return ��font��fontNameѹջ
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
*	\brief ʵ��Text:setText(text)
*
*	������Ҫ���Ƶ��ı�,���봫����ַ�����utf8��ʽ
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
*	\brief ʵ��Text:setFontColor({r,g,b,a})
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
*	\brief ʵ��Text:getFontColor()
*	\return ����r,g,b,a
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

