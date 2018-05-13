#pragma once

#include<string>

#include"lua\luaTools.h"

class UIStage;

namespace gui
{
	/** module define */
	static std::string ui_lua_widget_name = "Widget";
	static std::string ui_Lua_button_name = "Button";
	static std::string ui_lua_frame_name  = "Frame";

	/** system func */
	static int frame_api_get_root(lua_State* l);
	static int frame_api_create(lua_State* l);

	/** create func */
	static int frame_api_create_frame(lua_State* l);
	static int frame_api_create_button(lua_State* l);
}