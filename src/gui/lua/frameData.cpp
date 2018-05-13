#include "frameData.h"
#include "gui\widget\grid.h"

static std::map<std::string, int> WidgetAlignMapping = {
	{ "top",     gui::ALIGN_VERTICAL_TOP },
	{ "center",  gui::ALIGN_VERTICAL_CENTER },
	{ "bottom",  gui::ALIGN_VERTICAL_BOTTOM },
	{ "left",    gui::ALIGN_VERTICAL_TOP },
	{ "right",   gui::ALIGN_VERTICAL_BOTTOM },
};

int FrameData::mIndex = 0;

const std::map<gui::WIDGET_TYPE, const FrameData::FieldDescriptions> frameTypeDescriptions = {
	{ gui::WIDGET_TYPE::WIDGET_FRAME,{}	}
};

FrameData::DefaultValue::DefaultValue() :
	mType(VALUE_TYPE_STRING),
	mIntValue(0),
	mStringValue("")
{
}
FrameData::DefaultValue::DefaultValue(int value) :
	mType(VALUE_TYPE_INTEGER),
	mIntValue(value),
	mStringValue("")
{
}
FrameData::DefaultValue::DefaultValue(const string & value) :
	mType(VALUE_TYPE_STRING),
	mIntValue(0),
	mStringValue(value)
{
}
FrameData::DefaultValue::DefaultValue(bool value) :
	mType(VALUE_TYPE_BOOLEAN),
	mIntValue(static_cast<int>(value)),
	mStringValue("")
{
}

FrameData::FrameData():
	mType(gui::WIDGET_TYPE::WIDGET_UNKNOW)
{
}

FrameData FrameData::CheckFrameData(lua_State * l, int index, gui::WIDGET_TYPE type)
{
	LuaTools::CheckType(l, index, LUA_TTABLE);

	const std::string name = LuaTools::CheckFieldStringByDefault(l, index, "name", 
		std::to_string(FrameData::mIndex++));
	Point2 pos = LuaTools::CheckFieldPoint2ByDefault(l, index, "pos", Point2());
	Point2 gridPos = LuaTools::CheckFieldPoint2ByDefault(l, index, "grid_pos", Point2());
	Point2 size = LuaTools::CheckFieldPoint2ByDefault(l, index, "size", Point2());

	const std::string& verticalAlign = LuaTools::CheckFieldStringByDefault(l, index, "vertical", "top");
	const std::string& horizontalAlign = LuaTools::CheckFieldStringByDefault(l, index, "horizontal", "left");
	int flag = WidgetAlignMapping[verticalAlign];
	flag |= WidgetAlignMapping[horizontalAlign] << gui::ALIGN_HORIZONTAL_SHIFT;

	FrameData frameData;
	frameData.SetType(type);
	frameData.SetPos(pos);
	frameData.SetSize({ size.x, size.y });
	frameData.SetAlignFlag(flag);

	return frameData;
}

bool FrameData::ImportFromLua(lua_State * l)
{
	return false;
}
