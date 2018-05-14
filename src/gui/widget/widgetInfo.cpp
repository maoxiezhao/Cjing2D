#include"gui\widget\widgetInfo.h"


const std::string EnumInfoTraits<gui::WIDGET_CALL_BACK_TYPE>::prettyName = "entity type";
const EnumInfo<gui::WIDGET_CALL_BACK_TYPE>::nameType EnumInfoTraits<gui::WIDGET_CALL_BACK_TYPE>::names =
{
	{ gui::WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_ENTER,"OnMouseEnter"},
	{ gui::WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_LEAVE,"OnMouseLeave"},
	{ gui::WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_DOWN, "OnMouseDown"},
	{ gui::WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_UP,	"OnMouseUp" },
	{ gui::WIDGET_CALL_BACK_TYPE::WIDGET_ON_MOUSE_CLICK,"OnMouseClick" }
};