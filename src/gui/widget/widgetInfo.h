#pragma once

#include"game\enumInfo.h"

#include<string>

namespace gui
{
	enum class WIDGET_TYPE {
		WIDGET_UNKNOW,
		WIDGET_FRAME,
		WIDGET_BUTTON,
		WIDGET_IMAGE,
		WIDGET_LABEL,
	};

	enum class WIDGET_CALL_BACK_TYPE
	{
		WIDGET_ON_MOUSE_ENTER,
		WIDGET_ON_MOUSE_LEAVE,
		WIDGET_ON_MOUSE_HOVER,
		WIDGET_ON_MOUSE_DOWN,
		WIDGET_ON_MOUSE_UP,
		WIDGET_ON_MOUSE_CLICK,
		WIDGET_ON_MOUSE_DOUBLE_CLICK,
	};

}
// 用于获取key枚举值对应的字符
template<>
struct EnumInfoTraits<gui::WIDGET_CALL_BACK_TYPE>
{
	static const std::string prettyName;
	static const EnumInfo<gui::WIDGET_CALL_BACK_TYPE>::nameType names;
};

